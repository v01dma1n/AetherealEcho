#define ESP32DEBUGGING
#include <ESP32Logger.h>
#include <ESPping.h>

#include "ae_fsm_config.h"
#include "ae_fsm_execute.h"
#include "access_point.h"
#include "aethereal_echo.h"
#include "app_pref.h"
#include "melody.h"
#include "wifi_connect.h"

///////////////////////////////////////////////////////////////////////////////

#define MAX_WIFI_ALERT_COUNT 10

#ifdef DEBUG_SIMULATED_PING_SERVER
#define MAX_PING_ALERT_COUNT 3
#else
#define MAX_PING_ALERT_COUNT 10
#endif

unsigned wifiAlertCount{0};
unsigned pingAlertCount{0};
bool pingSoundOn{true};
bool alertOn{true};
bool pingServerColor{0};


void nextDNSSever() {
    String ipStr = appPrefs.nextPingIP();
    appPrefs.putPreferences();  // save the change
    DBGLOG(Info, "New ping server: %s", ipStr.c_str());
};

float pingServer(IPAddress remoteIP) {
    float pingAvg {0};
    // only once because more pings hold the loop too long
    bool res = Ping.ping(remoteIP, 1);
    if (res)
        pingAvg = Ping.averageTime();
    else
        pingAvg = 0; // this is failure
    return pingAvg;
}
void fsm_tick() {
   // DBGLOG(Debug, "FSM State: %s", fsm.getState()->getName().c_str());
};


void on_enter_boot() {
    wifiAlertCount = 0;
    pingSoundOn = appPrefs.config.pingSoundOn;
    alertOn = appPrefs.config.alertOn;
    if (WiFiConnect(AP_HOST_NAME, appPrefs.config.ssid,
                    appPrefs.config.password, WIFI_CONN_ATTEMPTS)) {
        playMelodyAsync(GPIO_BUZZER, MELODY_WIFI_CONNECTED);
        fsm.trigger(tr_wifi_connected);
    } else {
        playMelodyAsync(GPIO_BUZZER, MELODY_WIFI_DISCONNECTED);
        fsm.trigger(tr_wifi_not_connected);
    }
};

void on_enter_wifi_connected() {
    DBGLOG(Debug, ".");
    pingAlertCount = 0;
    // let the ping light fade out
    pingRGBTimer1.fadeOut(1000);
    pingRGBTimer2.fadeOut(1000);
};

void on_enter_ap_running() {
    DBGLOG(Debug, ".");
    setupAP(AP_HOST_NAME);
    playMelodyAsync(GPIO_BUZZER, MELODY_AP_STARTED);
    String msg = (String) "CONNECT TO '" + AP_HOST_NAME +
                   (String) "' WIFI AND CONFIGURE ACCESS";
    DBGLOG(Info, "%s\n", msg.c_str());
    wifiActive = false;
};

void on_enter_ping() {
    IPAddress pingIP = appPrefs.getCurPingIPAddress();
    DBGLOG(Info, "Ping server: %s", pingIP.toString().c_str());
    if (WiFi.status() == WL_CONNECTED) {
        DBGLOG(Debug, "WiFi Status: %d", WiFi.status());
        float pingAverage = pingServer(pingIP);
        if (pingAverage > 0) {
            // limit the range for the logarithmic output
            if (pingAverage < 1)
                pingAverage = 1;
            else if (pingAverage > 1000)
                pingAverage = 1000;
            meterOutTimer.setOutput(pingAverage);
            int pingIPIndex = appPrefs.getCurPingIPIndex();
            RGB pingIPColor = pingIPColors[pingIPIndex - 1];
            // light up the LEDs at once, which will fade away afterwards
            pingRGBTimer1.setTransitionMs(0);
            pingRGBTimer1.setOutput(pingIPColor.r, pingIPColor.g, pingIPColor.b);
            pingRGBTimer2.setTransitionMs(0);
            pingRGBTimer2.setOutput(pingIPColor.r, pingIPColor.g, pingIPColor.b);
            if (pingSoundOn)
              playMelodyAsync(GPIO_BUZZER, MELODY_PING);
            pingAlertCount = 0; // reset alert counter
            fsm.trigger(tr_ping_ok);
        } else { // ping failed
            fsm.trigger(tr_ping_timeout);
        }
    } else { // WiFi not connnected
        DBGLOG(Debug, "WiFi NOT Connected. Status: %d", WiFi.status());
        fsm.trigger(tr_wifi_lost);
    }
};

void on_enter_ping_alert() {
  // DBGLOG(Debug, "pingAlertCount=%d", pingAlertCount);
    pingAlertCount++;
    playMelodyAsync(GPIO_BUZZER, MELODY_PING_TIMEOUT);
     // restart the device if too many ping alerts occured
    if (pingAlertCount > MAX_PING_ALERT_COUNT) {
      nextDNSSever();
        // this should reboot the device
       fsm.trigger(tr_ping_timeout_max);
    }
   // meterValue = 0;
};

void on_state_ping_alert() {
  // DBGLOG(Debug, "pingAlertCount=%d", pingAlertCount);
};

void on_enter_ip_select() {
    DBGLOG(Debug, ".");
    playMelodyAsync(GPIO_BUZZER, MELODY_DNS_SELECTION_MODE);
    int pingIPNum = appPrefs.getCurPingIPIndex();
     RGB pingIPColor = pingIPColors[pingIPNum - 1];
    pingRGBTimer1.setTransitionMs(0);
    pingRGBTimer1.setOutput(pingIPColor.r, pingIPColor.g, pingIPColor.b);
    pingRGBTimer2.setTransitionMs(0);
    pingRGBTimer2.setOutput(pingIPColor.r, pingIPColor.g, pingIPColor.b);
  // timedTransitions[TTR_IP_SELECT_TO_WIFI_CONNECTED].reset(); // reset timed
    // transition out of this state
};

void on_exit_ip_select() { DBGLOG(Debug, "."); };

void on_enter_ip_next() {
   DBGLOG(Info, "Selecting next DNS server");
    playMelodyAsync(GPIO_BUZZER, MELODY_DNS_NEXT_SELECTED);
    nextDNSSever();
    // 2025-01-12 #issue001
    // the resed does not seem to work as the timed transition still is effective.
    // not sure if this is a but in the library or just wrong expectations
     timedTransitions[TTR_IP_SELECT_TO_WIFI_CONNECTED]
        .reset();  // reset timed transition out of this state
    fsm.trigger(tr_ip_select_done);
};


void on_enter_ping_sound_select() {
    DBGLOG(Debug, ".");
   // playMelodyAsync(GPIO_BUZZER, MELODY_DNS_SELECTION_MODE);
    // get the current state of the ping_sound
    pingSoundOn = appPrefs.config.pingSoundOn;
    if (pingSoundOn) {
        DBGLOG(Info, "Ping Sound is now: ON");
        meterOutTimer.setOutput(AE_OUTPUT_YES); // set the meter to point to Yes
        playMelodyAsync(GPIO_BUZZER, MELODY_SETTING_ON);
    } else {
        DBGLOG(Info, "Ping Sound is now: OFF");
        meterOutTimer.setOutput(AE_OUTPUT_NO); // set the meter to point to No
         playMelodyAsync(GPIO_BUZZER, MELODY_SETTING_OFF);
    }
};

void on_enter_ping_sound_toggle() {
    DBGLOG(Debug, ".");
    // get the current state of the ping_sound
    pingSoundOn = appPrefs.config.pingSoundOn;
    // flip the current state of the ping sound
    appPrefs.config.pingSoundOn = !appPrefs.config.pingSoundOn;
    // save connfiguration
    appPrefs.putPreferences();
    // 2025-01-12 #issue001
    // the resed does not seem to work as the timed transition still is effective.
    // not sure if this is a but in the library or just wrong expectations
       timedTransitions[TTR_PING_SOUND_SELECT_TO_WIFI_CONNECTED]
        .reset();  // reset timed transition out of this state
   fsm.trigger(tr_ping_sound_select_done);
};

;

void on_enter_alert_enabled_select() {
    DBGLOG(Debug, ".");
    // get the current state of the ping_sound
    alertOn = appPrefs.config.alertOn;
    if (alertOn) {
        DBGLOG(Info, "Alert is now: ON");
        meterOutTimer.setOutput(AE_OUTPUT_YES); // set the meter to point to Yes
        playMelodyAsync(GPIO_BUZZER, MELODY_SETTING_ON);
    } else {
       DBGLOG(Info, "Alert is now: OFF");
        meterOutTimer.setOutput(AE_OUTPUT_NO); // set the meter to point to No
        playMelodyAsync(GPIO_BUZZER, MELODY_SETTING_OFF);
    }
};

void on_enter_alert_enabled_toggle() {
    DBGLOG(Debug, ".");
    // get the current state of the ping_sound
   alertOn = appPrefs.config.alertOn;
    // flip the current state of the ping sound
    appPrefs.config.alertOn = !appPrefs.config.alertOn;
    // save connfiguration
    appPrefs.putPreferences();
};

;

void on_enter_wifi_alert() {
  DBGLOG(Debug, ".");
    wifiAlertCount++;
     DBGLOG(Debug, "wifiAlertCount: %d", wifiAlertCount);
    // show WiFi Disconnect Alert
    // meterValue = 0;
};

bool wifiAlertOn {false};

void on_enter_wifi_alert_toggle(){
    wifiAlertOn = !wifiAlertOn;
    if (wifiAlertOn) {
          // turn on red instantaneously
        pingRGBTimer1.setTransitionMs(0);
        pingRGBTimer1.setOutput(1, 0, 0);
         // fade away blue
       pingRGBTimer2.setTransitionMs(WIFI_ALERT_TOGGLE_MILLIS);
      pingRGBTimer2.setOutput(0, 0, 0);
    }
    else {
          // fade away red
      pingRGBTimer1.setTransitionMs(WIFI_ALERT_TOGGLE_MILLIS);
       pingRGBTimer1.setOutput(0, 0, 0);
       // turn on blue instantaneously
        pingRGBTimer2.setTransitionMs(0);
       pingRGBTimer2.setOutput(0, 0, 1);
    };
    playMelodyAsync(GPIO_BUZZER, MELODY_WIFI_DISCONNECTED);
   fsm.trigger(tr_wifi_alert_toggle_end);
};



void on_enter_wifi_disconnected() {
    DBGLOG(Debug, ".");
   pingRGBTimer1.off();
  pingRGBTimer2.off();
   if (WiFiConnect(AP_HOST_NAME, appPrefs.config.ssid,
                    appPrefs.config.password, WIFI_CONN_ATTEMPTS)) {
        playMelodyAsync(GPIO_BUZZER, MELODY_WIFI_CONNECTED);
       fsm.trigger(tr_wifi_connected);
    } else
        fsm.trigger(tr_wifi_not_connected);
};

void on_enter_reboot() {
    DBGLOG(Debug, ".");
    DBGLOG(Info, "Rebooting...");
    // the melody must be synchronous to be played before the reboot
    playMelody(GPIO_BUZZER, MELODY_SYSTEM_RESTART);
   ESP.restart();
};

