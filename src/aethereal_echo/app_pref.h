#ifndef APP_PREF_H
#define APP_PREF_H

#define ESP32DEBUGGING
#include <Arduino.h>
#include <IPAddress.h>
#include <Preferences.h>

#define PING_IP_COUNT 4
#define MAX_PREF_STRING_LEN 64

struct AppConfig {
  char ssid[MAX_PREF_STRING_LEN];
  char password[MAX_PREF_STRING_LEN];
  char time_zone[MAX_PREF_STRING_LEN];
  char pingIPs[PING_IP_COUNT]
              [MAX_PREF_STRING_LEN]; // Array of char arrays for 4 IP addresses
  int32_t curPingIPIndex;
  int32_t pingIntervalSec;
  bool pingSoundOn;
  bool alertOn;
  int32_t logLevel;
};

class AppPreferences {
protected:
  Preferences prefs;

public:
  AppConfig config;

  void setcurPingIPIndex(unsigned newPingIPNum) {
    if (newPingIPNum < 1)
      newPingIPNum = 1;
    else if (newPingIPNum > PING_IP_COUNT)
      newPingIPNum = PING_IP_COUNT;
    config.curPingIPIndex = newPingIPNum;
    putPreferences();
  };

  unsigned getCurPingIPIndex() { return config.curPingIPIndex; };

  const char *getCurPingIP() {
    if (config.curPingIPIndex <= PING_IP_COUNT) {
      return config.pingIPs[config.curPingIPIndex - 1];
    } else {
      return "127.0.0.1";
    }
  };

  IPAddress getCurPingIPAddress() {
    if (config.curPingIPIndex <= PING_IP_COUNT) {
      return IPAddress(config.pingIPs[config.curPingIPIndex - 1]);
    } else {
      return IPAddress("127.0.0.1");
    }
  };

  const char *nextPingIP() {
    config.curPingIPIndex++;
    if (config.curPingIPIndex > PING_IP_COUNT)
      config.curPingIPIndex = 1;
    putPreferences();
    return getCurPingIP();
  };

  void setup();
  void getPreferences();
  void putPreferences();
  void dumpPreferences();
};

extern AppPreferences appPrefs;

#endif // APP_PREF_H
