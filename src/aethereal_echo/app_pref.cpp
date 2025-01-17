#define ESP32DEBUGGING
#include <ESP32Logger.h>

#include "aethereal_echo.h"
#include "app_pref.h"
#include <Arduino.h>
#include <Preferences.h>
#include <string.h>

#define APP_PREF_WIFI_SSID "wifi_ssid"
#define APP_PREF_PASSWORD "password"
#define APP_PREF_TIME_ZONE "time_zone"
#define APP_PREF_PING_IP1 "ping_ip1"
#define APP_PREF_PING_IP2 "ping_ip2"
#define APP_PREF_PING_IP3 "ping_ip3"
#define APP_PREF_PING_IP4 "ping_ip4"
#define APP_PREF_PING_IP_INDEX "ping_ip_index"
#define APP_PREF_PING_INTERVAL_SEC "ping_inerval"
#define APP_PREF_PING_SOUND_ON "ping_sound_on"
#define APP_PREF_ALERT_ON "alert_on"
#define APP_PREF_LOG_LEVEL "log_level"

AppPreferences appPrefs{};

void AppPreferences::setup() {
  prefs.begin("config");
  getPreferences();
}

void AppPreferences::getPreferences() {
  prefs.getString(APP_PREF_WIFI_SSID, config.ssid, sizeof(config.ssid));
  prefs.getString(APP_PREF_PASSWORD, config.password, sizeof(config.password));
  prefs.getString(APP_PREF_TIME_ZONE, config.time_zone,
                  sizeof(config.time_zone));

  prefs.getString(APP_PREF_PING_IP1, config.pingIPs[0],
                  sizeof(config.pingIPs[0]));
  prefs.getString(APP_PREF_PING_IP2, config.pingIPs[1],
                  sizeof(config.pingIPs[1]));
  prefs.getString(APP_PREF_PING_IP3, config.pingIPs[2],
                  sizeof(config.pingIPs[2]));
  prefs.getString(APP_PREF_PING_IP4, config.pingIPs[3],
                  sizeof(config.pingIPs[3]));

  config.curPingIPIndex = prefs.getInt(APP_PREF_PING_IP_INDEX, 1);
  if (config.curPingIPIndex < 1)
    config.curPingIPIndex = 1;
  else if (config.curPingIPIndex > PING_IP_COUNT)
    config.curPingIPIndex = PING_IP_COUNT;

  config.pingIntervalSec = prefs.getInt(APP_PREF_PING_INTERVAL_SEC, 3);
  if (config.pingIntervalSec < 3)
    config.pingIntervalSec = 3;
  else if (config.pingIntervalSec > MAX_PING_INTERVAL_SEC)
    config.pingIntervalSec = MAX_PING_INTERVAL_SEC;

  config.pingSoundOn = prefs.getBool(APP_PREF_PING_SOUND_ON, true);
  config.alertOn = prefs.getBool(APP_PREF_ALERT_ON, true);
  config.logLevel =
      prefs.getInt(APP_PREF_LOG_LEVEL, int32_t(ESP32LogLevel::Info));

  if (config.logLevel < int32_t(ESP32LogLevel::Error))
    config.logLevel = int32_t(ESP32LogLevel::Error);
  else if (config.logLevel > int32_t(ESP32LogLevel::Debug))
    config.logLevel = int32_t(ESP32LogLevel::Debug);

  setenv("TZ", config.time_zone, 1);
  tzset();
}

void AppPreferences::putPreferences() {
  prefs.putString(APP_PREF_WIFI_SSID, config.ssid);
  prefs.putString(APP_PREF_PASSWORD, config.password);
  prefs.putString(APP_PREF_TIME_ZONE, config.time_zone);
  prefs.putString(APP_PREF_PING_IP1, config.pingIPs[0]);
  prefs.putString(APP_PREF_PING_IP2, config.pingIPs[1]);
  prefs.putString(APP_PREF_PING_IP3, config.pingIPs[2]);
  prefs.putString(APP_PREF_PING_IP4, config.pingIPs[3]);
  prefs.putInt(APP_PREF_PING_IP_INDEX, config.curPingIPIndex);
  prefs.putInt(APP_PREF_PING_INTERVAL_SEC, config.pingIntervalSec);
  prefs.putBool(APP_PREF_PING_SOUND_ON, config.pingSoundOn);
  prefs.putBool(APP_PREF_ALERT_ON, config.alertOn);
  prefs.putInt(APP_PREF_LOG_LEVEL, config.logLevel);
}

void AppPreferences::dumpPreferences() {
  DBGLOG(Debug, "%s: %s", APP_PREF_WIFI_SSID, config.ssid);
  DBGLOG(Debug, "%s: %s", APP_PREF_PASSWORD, "***");
  DBGLOG(Debug, "%s: %s", APP_PREF_TIME_ZONE, config.time_zone);
  DBGLOG(Debug, "%s: %s", APP_PREF_PING_IP1, config.pingIPs[0]);
  DBGLOG(Debug, "%s: %s", APP_PREF_PING_IP2, config.pingIPs[1]);
  DBGLOG(Debug, "%s: %s", APP_PREF_PING_IP3, config.pingIPs[2]);
  DBGLOG(Debug, "%s: %s", APP_PREF_PING_IP4, config.pingIPs[3]);
  DBGLOG(Debug, "%s: %d", APP_PREF_PING_IP_INDEX, config.curPingIPIndex);
  DBGLOG(Debug, "%s: %d", APP_PREF_PING_INTERVAL_SEC, config.pingIntervalSec);
  DBGLOG(Debug, "%s: %s", APP_PREF_PING_SOUND_ON,
         config.pingSoundOn ? "Y" : "N");
  DBGLOG(Debug, "%s: %s", APP_PREF_ALERT_ON, config.alertOn ? "Y" : "N");
  DBGLOG(Debug, "%s: %d", APP_PREF_LOG_LEVEL, config.logLevel);
}
