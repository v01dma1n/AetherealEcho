#define ESP32DEBUGGING
#include <ESP32Logger.h>
#include <ESPping.h>

#include "access_point.h"
#include "ae_state_machine.h"
#include "aethereal_echo.h"
#include "app_pref.h"
#include "melody.h"
#include "wifi_connect.h"

SimpleFSM fsm;

///////////////////////////////////////////////////////////////////////////////

#define MAX_WIFI_ALERT_COUNT 10

#ifdef DEBUG_SIMULATED_PING_SERVER
#define MAX_PING_ALERT_COUNT 3
#else
#define MAX_PING_ALERT_COUNT 10
#endif

unsigned wifiAlertCount{0};
unsigned pingTimeoutCount{0};
unsigned pingAlertCount{0};
bool pingSoundOn{true};
bool alertOn{true};
bool pingServerColor{0};

void nextDNSSever() {
  String ipStr = appPrefs.nextPingIP();
  appPrefs.putPreferences(); // save the change
  DBGLOG(Info, "New ping server: %s", ipStr.c_str());
};

float pingServer(IPAddress remoteIP) {
  float pingAvg{0};
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

  pingSoundOn = appPrefs.config.pingSoundOn;
  alertOn = appPrefs.config.alertOn;

  if (WiFiConnect(AP_HOST_NAME, appPrefs.config.ssid, appPrefs.config.password,
                  WIFI_CONN_ATTEMPTS)) {
    playMelodyAsync(GPIO_BUZZER, MELODY_WIFI_CONNECTED);
    fsm.trigger(tr_wifi_connected);
  } else
    fsm.trigger(tr_wifi_not_connected);
};

void on_enter_wifi_connected() {
  DBGLOG(Debug, ".");
  pingTimeoutCount = 0;

  // let the ping light fade out
  pingRGBTimer1.fadeOut(1000);
  pingRGBTimer2.fadeOut(1000);
};

void on_enter_ap_running() {
  DBGLOG(Debug, ".");
  setupAP(AP_HOST_NAME);
  meterOutTimer.setOutput(AE_AP_RUNNING);
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

      pingTimeoutCount = 0; // reset alert counter
      fsm.trigger(tr_ping_ok);

    } else { // ping failed
      fsm.trigger(tr_ping_timeout);
    }
  } else { // WiFi not connnected
    DBGLOG(Debug, "WiFi NOT Connected. Status: %d", WiFi.status());
    fsm.trigger(tr_wifi_lost);
  }
};

void on_enter_ping_timeout() {
  DBGLOG(Debug, ".");
  pingTimeoutCount++;
  pingAlertCount = 0;
  meterOutTimer.setOutput(AE_PING_TIMEOUT);
  playMelodyAsync(GPIO_BUZZER, MELODY_PING_TIMEOUT);

  // restart the device if too many ping alerts occured
  if (pingTimeoutCount > MAX_PING_ALERT_COUNT) {
    nextDNSSever();
    // this should start ping alert
    fsm.trigger(tr_ping_timeout_max);
  }

  // meterValue = 0;
};

void on_enter_ping_alert() {
  DBGLOG(Debug, ".");
  meterOutTimer.setOutput(AE_PING_TIMEOUT);
  pingAlertCount++;
  DBGLOG(Debug, "pingAlertCount: %d", pingAlertCount);
};

bool pingAlertOn{true};

void on_enter_ping_alert_switch() {
  DBGLOG(Debug, ".");
  pingAlertOn = !pingAlertOn;

  if (pingAlertOn) {
    // turn on red instantaneously
    pingRGBTimer1.setTransitionMs(0);
    pingRGBTimer1.setOutput(1, 0, 0);
    // fade away blue
    pingRGBTimer2.setTransitionMs(PING_ALERT_SWITCH_MILLIS);
    pingRGBTimer2.setOutput(0, 0, 0);
  } else {
    // fade away red
    pingRGBTimer1.setTransitionMs(PING_ALERT_SWITCH_MILLIS);
    pingRGBTimer1.setOutput(0, 0, 0);
    // turn on blue instantaneously
    pingRGBTimer2.setTransitionMs(0);
    pingRGBTimer2.setOutput(0, 0, 1);
  };
  if (alertOn)
    playMelodyAsync(GPIO_BUZZER, MELODY_PING_ALERT);

  fsm.trigger(tr_ping_alert_switch_end);
};

void on_enter_ip_select() {
  DBGLOG(Debug, ".");
  playMelodyAsync(GPIO_BUZZER, MELODY_DNS_SELECTION_MODE);

  int pingIPNum = appPrefs.getCurPingIPIndex();
  meterOutTimer.setOutput(pingIPMeter[pingIPNum]);
  RGB pingIPColor = pingIPColors[pingIPNum - 1];
  pingRGBTimer1.setTransitionMs(0);
  pingRGBTimer1.setOutput(pingIPColor.r, pingIPColor.g, pingIPColor.b);
  pingRGBTimer2.setTransitionMs(0);
  pingRGBTimer2.setOutput(pingIPColor.r, pingIPColor.g, pingIPColor.b);

  // timedTransitions[TTR_IP_SELECT_TO_WIFI_CONNECTED].reset(); // reset timed
  // transition out of this state
};

void on_enter_ip_next() {
  DBGLOG(Info, "Selecting next DNS server");

  nextDNSSever();

  // 2025-01-12 #issue001
  // the resed does not seem to work as the timed transition still is effective.
  // not sure if this is a but in the library or just wrong expectations
  timedTransitions[TTR_IP_SELECT_TO_WIFI_CONNECTED]
      .reset(); // reset timed transition out of this state
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

void on_enter_ping_sound_switch() {
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
      .reset(); // reset timed transition out of this state
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

void on_enter_alert_enabled_switch() {
  DBGLOG(Debug, ".");

  // get the current state of the ping_sound
  alertOn = appPrefs.config.alertOn;
  // flip the current state of the ping sound
  appPrefs.config.alertOn = !appPrefs.config.alertOn;
  // save connfiguration
  appPrefs.putPreferences();
};

void on_enter_wifi_alert() {
  DBGLOG(Debug, ".");
  meterOutTimer.setOutput(AE_NO_WIFI);

  wifiAlertCount++;
  DBGLOG(Debug, "wifiAlertCount: %d", wifiAlertCount);
};

bool wifiAlertOn{false};

// play alarm with red/blue flashing
void on_enter_wifi_alert_switch() {
  wifiAlertOn = !wifiAlertOn;

  if (wifiAlertOn) {
    // turn on red instantaneously
    pingRGBTimer1.setTransitionMs(0);
    pingRGBTimer1.setOutput(1, 0, 0);
    // fade away blue
    pingRGBTimer2.setTransitionMs(WIFI_ALERT_SWITCH_MILLIS);
    pingRGBTimer2.setOutput(0, 0, 0);
  } else {
    // fade away red
    pingRGBTimer1.setTransitionMs(WIFI_ALERT_SWITCH_MILLIS);
    pingRGBTimer1.setOutput(0, 0, 0);
    // turn on blue instantaneously
    pingRGBTimer2.setTransitionMs(0);
    pingRGBTimer2.setOutput(0, 0, 1);
  };
  if (alertOn)
    playMelodyAsync(GPIO_BUZZER, MELODY_WIFI_DISCONNECTED);

  fsm.trigger(tr_wifi_alert_switch_end);
};

void on_enter_wifi_disconnected() {
  DBGLOG(Debug, ".");

  pingRGBTimer1.off();
  pingRGBTimer2.off();

  if (WiFiConnect(AP_HOST_NAME, appPrefs.config.ssid, appPrefs.config.password,
                  WIFI_CONN_ATTEMPTS)) {
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

State s[NUM_OF_STATES] = {
    State("boot", on_enter_boot),
    State("wifi_connected", on_enter_wifi_connected),
    State("ap_running", on_enter_ap_running),
    State("ping", on_enter_ping),
    State("ping_timeout", on_enter_ping_timeout),
    State("ping_alert", on_enter_ping_alert),
    State("ping_alert_switch", on_enter_ping_alert_switch),
    State("ip_select", on_enter_ip_select),
    State("ip_next", on_enter_ip_next),
    State("ping_sound_select", on_enter_ping_sound_select),
    State("ping_sound_switch", on_enter_ping_sound_switch),
    State("alert_enabled_select", on_enter_alert_enabled_select),
    State("alert_enabled_switch", on_enter_alert_enabled_switch),
    State("wifi_alert", on_enter_wifi_alert),
    State("wifi_alert_switch", on_enter_wifi_alert_switch),
    State("wifi_disconnected", on_enter_wifi_disconnected),
    State("reboot", on_enter_reboot)};

Transition transitions[] = {
    Transition(&s[ST_BOOT], &s[ST_WIFI_CONNECTED], tr_wifi_connected, NULL,
               "tr_wifi_connected"),
    Transition(&s[ST_BOOT], &s[ST_AP_RUNNING], tr_wifi_not_connected, NULL,
               "tr_wifi_not_connected"),
    Transition(&s[ST_AP_RUNNING], &s[ST_REBOOT], tr_ap_config_ok, NULL,
               "trp_config_ok"),
    Transition(&s[ST_AP_RUNNING], &s[ST_REBOOT], tr_ap_long_press, NULL,
               "tr_ap_long_press"),
    Transition(&s[ST_AP_RUNNING], &s[ST_AP_RUNNING], tr_ap_config_err, NULL,
               "tr_ap_config_err"),
    Transition(&s[ST_WIFI_CONNECTED], &s[ST_AP_RUNNING], tr_ap_long_press, NULL,
               "tr_ap_long_press"),

    Transition(&s[ST_PING], &s[ST_PING_TIMEOUT], tr_ping_timeout, NULL,
               "tr_ping_timeout"),
    Transition(&s[ST_PING], &s[ST_WIFI_CONNECTED], tr_ping_ok, NULL,
               "tr_ping_ok"),
    Transition(&s[ST_PING], &s[ST_WIFI_ALERT], tr_wifi_lost, NULL,
               "tr_wifi_lost"),

    Transition(&s[ST_PING_ALERT], &s[ST_WIFI_CONNECTED], tr_ping_alert_end,
               NULL, "tr_ping_alert_end"),

    Transition(&s[ST_PING_TIMEOUT], &s[ST_PING_ALERT], tr_ping_timeout_max,
               NULL, "tr_ping_timeout_max"),
    Transition(&s[ST_PING_ALERT], &s[ST_REBOOT], tr_ping_timeout_max, NULL,
               "tr_ping_timeout_max"),
    Transition(&s[ST_PING_ALERT], &s[ST_IP_SELECT], tr_ip_select_long_press,
               NULL, "tr_ip_select_long_press"),
    Transition(&s[ST_PING_ALERT], &s[ST_AP_RUNNING], tr_ap_long_press, NULL,
               "tr_ap_long_press"),
    Transition(&s[ST_PING_ALERT_SWITCH], &s[ST_PING_ALERT],
               tr_ping_alert_switch_end, NULL, "tr_ping_alert_switch_end"),

    // select IP for pinging
    Transition(&s[ST_WIFI_CONNECTED], &s[ST_IP_SELECT], tr_ip_select_long_press,
               NULL, "tr_ip_select_long_press"),
    Transition(&s[ST_IP_SELECT], &s[ST_IP_NEXT], tr_ip_select_short_press, NULL,
               "tr_ip_select_short_press"),
    Transition(&s[ST_IP_SELECT], &s[ST_WIFI_CONNECTED], tr_ip_select_long_press,
               NULL, "tr_ip_select_long_press"),
    Transition(&s[ST_IP_NEXT], &s[ST_IP_SELECT], tr_ip_select_done, NULL,
               "tr_ip_select_done"),

    // ping sound on or off
    Transition(&s[ST_WIFI_CONNECTED], &s[ST_PING_SOUND_SELECT],
               tr_ping_sound_long_press, NULL, "tr_ping_sound_long_press"),
    Transition(&s[ST_PING_SOUND_SELECT], &s[ST_PING_SOUND_SWITCH],
               tr_ping_sound_short_press, NULL, "tr_ping_sound_short_press"),
    Transition(&s[ST_PING_SOUND_SELECT], &s[ST_WIFI_CONNECTED],
               tr_ping_sound_long_press, NULL, "tr_ping_sound_long_press"),
    Transition(&s[ST_PING_SOUND_SWITCH], &s[ST_PING_SOUND_SELECT],
               tr_ping_sound_select_done, NULL, "tr_ping_sound_select_done"),

    // alert enabled on or off
    Transition(&s[ST_WIFI_CONNECTED], &s[ST_ALERT_ENABLED_SELECT],
               tr_alert_enabled_long_press, NULL,
               "tr_alert_enabled_long_press"),
    Transition(&s[ST_ALERT_ENABLED_SELECT], &s[ST_ALERT_ENABLED_SWITCH],
               tr_alert_enabled_short_press, NULL,
               "tr_alert_enabled_short_press"),
    Transition(&s[ST_ALERT_ENABLED_SELECT], &s[ST_WIFI_CONNECTED],
               tr_alert_enabled_long_press, NULL,
               "tr_alert_enabled_long_press"),
    Transition(&s[ST_ALERT_ENABLED_SWITCH], &s[ST_ALERT_ENABLED_SELECT],
               tr_alert_enabled_select_done, NULL,
               "tr_alert_enabled_select_done"),

    Transition(&s[ST_WIFI_CONNECTED], &s[ST_WIFI_ALERT], tr_wifi_lost, NULL,
               "tr_wifi_lost"),
    Transition(&s[ST_WIFI_ALERT], &s[ST_WIFI_DISCONNECTED], tr_wifi_alert_end,
               NULL, "tr_wifi_alert_end"),
    Transition(&s[ST_WIFI_ALERT_SWITCH], &s[ST_WIFI_ALERT],
               tr_wifi_alert_switch_end, NULL, "tr_wifi_alert_switch_end"),
    Transition(&s[ST_WIFI_DISCONNECTED], &s[ST_WIFI_CONNECTED],
               tr_wifi_reconnected, NULL, "tr_wifi_reconnected"),

};

TimedTransition timedTransitions[NUM_OF_TTR] = {
    TimedTransition(&s[ST_AP_RUNNING], &s[ST_REBOOT],
                    AP_RUNNING_TO_REBOOT), // Acess Point Timeout
    TimedTransition(&s[ST_WIFI_CONNECTED], &s[ST_PING],
                    WIFI_CONNECTED_TO_PING_MILLIS), // how frequently to ping

    TimedTransition(&s[ST_PING_TIMEOUT], &s[ST_PING],
                    PING_TIMEOUT_MILLIS), // how long ping alert lastas
    TimedTransition(&s[ST_PING_ALERT], &s[ST_PING_ALERT_SWITCH],
                    PING_ALERT_SWITCH_MILLIS),
    TimedTransition(&s[ST_PING_ALERT], &s[ST_REBOOT], PING_ALERT_REBOOT_MILLIS),

    TimedTransition(
        &s[ST_IP_SELECT], &s[ST_WIFI_CONNECTED],
        IP_SELECT_TO_WIFI_CONNECTED_MILLIS), // IP selection mode timeout

    TimedTransition(&s[ST_PING_SOUND_SELECT], &s[ST_WIFI_CONNECTED],
                    PING_SOUND_SELECT_TO_WIFI_CONNECTED_MILLIS), // ping sound
                                                                 // enabled/disabbled
                                                                 // timeout
    TimedTransition(
        &s[ST_ALERT_ENABLED_SELECT], &s[ST_WIFI_CONNECTED],
        ALERT_ENABLED_SELECT_TO_WIFI_CONNECTED_MILLIS), // ping alert
                                                        // enabled/disabbled
                                                        // timeout

    TimedTransition(&s[ST_WIFI_ALERT], &s[ST_WIFI_ALERT_SWITCH],
                    WIFI_ALERT_SWITCH_MILLIS),
    TimedTransition(&s[ST_WIFI_ALERT], &s[ST_WIFI_DISCONNECTED],
                    WIFI_ALERT_TO_DISCONNECTED_MILLIS),

    TimedTransition(&s[ST_WIFI_DISCONNECTED], &s[ST_REBOOT],
                    WIFI_DISCONNECTED_TO_REBOOT_MILLIS),

};

int numTransitions = sizeof(transitions) / sizeof(Transition);
int numTimedTransitions = sizeof(timedTransitions) / sizeof(TimedTransition);

void aeFsmSetup() {

  // tweak the timed transitions based on the configuration
  timedTransitions[TTR_WIFI_CONNECTED_TO_PING].setup(
      &s[ST_WIFI_CONNECTED], &s[ST_PING],
      appPrefs.config.pingIntervalSec * 1000);

  fsm.add(transitions, numTransitions);
  fsm.add(timedTransitions, numTimedTransitions);
  fsm.setInitialState(&s[ST_BOOT]);
};

void aeFsmLoop() { fsm.run(FSM_RUN_MILLIS, fsm_tick); };

String aeShowGraph() {
  String message = "";
  message += F("<html>\n");
  message += F("<head>\n");
  message += F("<meta http-equiv='refresh' content='1'>\n");
  message += F("<title>GraphVizArt</title>\n");
  message += F("</head>\n");
  message += F("<body>\n");
  message +=
      F("<a href='http://gravizo.com/'><img src='https://g.gravizo.com/svg?");
  message += fsm.getDotDefinition();
  message += F("'/></a>");
  message += F("</body>\n");
  message += F("</html>\n");
  return message;
}
