#define ESP32DEBUGGING
#include <ESP32Logger.h>

#include "access_point.h"
#include "ae_fsm_config.h"
#include "aethereal_echo.h"
#include "app_pref.h"
#include "led_rgb.h"
#include "melody.h"
#include "meter_output.h"
#include "sntp_timer.h"
#include "touch_timer.h"
#include "version.h"

MeterOutput meterOutTimer(100, GPIO_PWM); // update output
PingRGB pingRGBTimer1(10, GPIO_LED1_R, GPIO_LED1_G, GPIO_LED1_B);
PingRGB pingRGBTimer2(10, GPIO_LED2_R, GPIO_LED2_G, GPIO_LED2_B);

RGB pingIPColors[PING_IP_COUNT];
float pingIPMeter[PING_IP_COUNT];

TimerMillis debugTimer(3000);

bool wifiActive = false;
int apReboot = 0;
// bool dnsSelectionActive = false;

int touchThreshold = 40;
bool touch1detected = false;
bool touch2detected = false;
bool touch3detected = false;
bool touch4detected = false;

TouchTimer touchAPTimer(TOUCH_TIMER_MILLIS, T4);
TouchTimer touchDNSTimer(TOUCH_TIMER_MILLIS, T5);
TouchTimer touchAlertEnabledTimer(TOUCH_TIMER_MILLIS, T6);
TouchTimer touchPingSoundTimer(TOUCH_TIMER_MILLIS, T7);

double colorRed = 0, colorGreen = 0, colorBlue = 0;

String ip2Str(IPAddress ip) {
  String s = "";
  for (int i = 0; i < 4; i++) {
    s += i ? "." + String(ip[i]) : String(ip[i]);
  }
  return s;
}

void setup() {

  // Serial.begin(115200);
  Serial.begin(19200);
  while (!Serial)
    delay(300);

  randomSeed(0);

  // Initialize debugging with timestamp since start (start = 0)
  DBGINI(&Serial, ESP32Timestamp::TimestampSinceStart)
  DBGLEV(Error);
  // Start debugging
  DBGSTA
  DBGLOG(Info, "Starting")

  appPrefs.setup();
  appPrefs.getPreferences();
  appPrefs.dumpPreferences();
  // change the log level based on the configuration
  Log.setLogLevel(ESP32LogLevel(appPrefs.config.logLevel));

  pingIPColors[0] = {1, 0, 0}; // Red
  pingIPColors[1] = {0, 1, 0}; // Green
  pingIPColors[2] = {0, 0, 1}; // Blue
  pingIPColors[3] = {1, 1, 1}; // White

  pingIPMeter[0] = AE_OUTPUT_BEACON1;
  pingIPMeter[1] = AE_OUTPUT_BEACON2;
  pingIPMeter[2] = AE_OUTPUT_BEACON3;
  pingIPMeter[3] = AE_OUTPUT_BEACON4;

  // appPrefs.ssid = "AE_WiFi";
  // appPrefs.password = "";
  // appPrefs.putPreferences();

  debugTimer.setCanRestart(true);
  debugTimer.timerStart();

  aeFsmSetup();
#ifdef DEBUG_FSM_GRAPH
  String fsmGraph = aeShowGraph();
  // DBGLOG(Debug, "%s\n", fsmGraph.c_str())
  Serial.println(fsmGraph.c_str());
#endif

  pinMode(GPIO_SWITCH, INPUT);
  pinMode(GPIO_SWITCH, INPUT_PULLUP);
  pinMode(GPIO_LAMP, OUTPUT);
  pinMode(GPIO_BUZZER, OUTPUT);

  meterOutTimer.setup();
  pingRGBTimer1.setup();
  pingRGBTimer2.setup();

  touchAPTimer.setup();
  touchAttachInterrupt(T7, gotTouch1, touchThreshold);
  touchAttachInterrupt(T6, gotTouch2, touchThreshold);
  touchAttachInterrupt(T5, gotTouch3, touchThreshold);
  touchAttachInterrupt(T4, gotTouch4, touchThreshold);

  meterOutTimer.timerOutputMillis.timerStart();
  pingRGBTimer1.timerOutputMillis.timerStart();
  pingRGBTimer2.timerOutputMillis.timerStart();

  touchAPTimer.timerMillis.timerStart();
  touchDNSTimer.timerMillis.timerStart();
  touchPingSoundTimer.timerMillis.timerStart();
  touchAlertEnabledTimer.timerMillis.timerStart();

  // Log an info-message
  DBGLOG(Info, "\n%s v.%s, %s, %s\n", APP_NAME, VERSION_STRING.c_str(),
         APP_DATE, APP_AUTHOR)

} // setup()

void gotTouch1() { touch1detected = true; }
void gotTouch2() { touch2detected = true; }
void gotTouch3() { touch3detected = true; }
void gotTouch4() { touch4detected = true; }

void loop() {

  aeFsmLoop();

  if (wifiActive) {
    if (timeAvail)
      loopSntpGetTime(NTP_REFRESH_INTERVAL_MILLIS);
  }

  // turn on and of the lamp based on the manual switch state
  int switchState = digitalRead(GPIO_SWITCH);
  digitalWrite(GPIO_LAMP, !switchState);

  pingRGBTimer1.loop();
  pingRGBTimer2.loop();

  meterOutTimer.loop();

  melody_loop();

  if (touch1detected) {
    touch1detected = false;
  }
  if (touch2detected) {
    touch2detected = false;
  }
  if (touch3detected) {
    touch3detected = false;
  }
  if (touch4detected) {
    touch4detected = false;
  }

  // Check if we need to activate the access point
  if (touchAPTimer.loop()) {
    if (touchAPTimer.isShortTouch) {
      DBGLOG(Debug, "Short Touch: AP");
    } else if (touchAPTimer.isLongTouch) {
      DBGLOG(Debug, "Long Touch: AP");
      fsm.trigger(tr_ap_long_press);
      touchAPTimer.reset();
    }
  }

  if (touchDNSTimer.loop()) {
    if (touchDNSTimer.isShortTouch) {
      DBGLOG(Debug, "Short Touch: DNS");
      fsm.trigger(tr_ip_select_short_press);
      touchDNSTimer.reset();
    } else if (touchDNSTimer.isLongTouch) {
      DBGLOG(Debug, "Long Touch: DNS");
      fsm.trigger(tr_ip_select_long_press);
      touchDNSTimer.reset();
    }
  }

  if (touchPingSoundTimer.loop()) {
    if (touchPingSoundTimer.isShortTouch) {
      DBGLOG(Debug, "Short Touch: ping sound");
      fsm.trigger(tr_ping_sound_short_press);
      touchPingSoundTimer.reset();
    } else if (touchPingSoundTimer.isLongTouch) {
      DBGLOG(Debug, "Long Touch: ping sound");
      fsm.trigger(tr_ping_sound_long_press);
      touchPingSoundTimer.reset();
    }
  }

  if (touchAlertEnabledTimer.loop()) {
    if (touchAlertEnabledTimer.isShortTouch) {
      DBGLOG(Debug, "Short Touch: alert enabled");
      fsm.trigger(tr_alert_enabled_short_press);
      touchAlertEnabledTimer.reset();
    } else if (touchAlertEnabledTimer.isLongTouch) {
      DBGLOG(Debug, "Long Touch: alert enabled");
      fsm.trigger(tr_alert_enabled_short_press);
      touchAlertEnabledTimer.reset();
    }
  }
}
