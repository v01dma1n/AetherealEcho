#define APP_NAME "Aethereal Echo"
#define APP_AUTHOR "v01dma1n"
#define APP_DATE "2025-01-18"

#define VER_MAJOR 0
#define VER_MINOR 22
#define VER_BUILD 0

#include <string>

#define VERSION_STRING                                                         \
  (std::to_string(VER_MAJOR) + "." + std::to_string(VER_MINOR) + "." +         \
   std::to_string(VER_BUILD))

/* Known Problems

  #issue001 The TimedTransition is not reset after NextIP selection and times
  out after predefined interval. This may be a problem in the library or due to
  incorrect use of reset() function.

  #issue002 No redirection from Captive Portal

  #issue004 Reconnect to WiFi after disconnection alert

  #issue006 Time Zone should be validated
  Does it even work correctly?

  #issue007 Touch sensors need some debouncing. After long touch, a short touch
  is immediately triggered. It is OK that the funcion turns on automatically
  during long touch (after required time elapses) but the short touch should be
  captured only after long touch ends.

  #issue012 AP seems to time out too early

  #issue013 "setenv("TZ", config.time_zone, 1);" should not bin in preferences modlue

  #issue014 State graph does not have a version information - nice to have
  This in aeFsmSetup() creates too long string, which does not fit the state cirle.
  s[ST_BOOT].setName(s[ST_BOOT].getName() + String(": ") + String(APP_NAME) + String(" v.") + String(VERSION_STRING.c_str()));
*/

/* Version history

2025-01-18 v.0.22 The FSM graph construction fixed
- The FSM graph construction fixed afer the code was lost.

2025-01-17 v.0.21 FSM is now split into config and execute
- FSM is now split into config and execute
- AE is now on GitHub


2025-01-15 v.0.20  Restored defaults for DNS servers
- Restored defaults for DNS servers, which were removed after String to char* conversion

2025-01-15 v.0.19 Meter reflects now the state
- Meter states for 'no wifi', 'ping timeout and 'AP running' added
- Ping alert added

2025-01-15 v.0.18 #issue011 Remove use of String wherever possible.
- Sting not completely removed from AP since there is always reboot after the AP


2025-01-14 v.0.17 refactoring & turn AP on when there is a ping alert
- #issue010 Unable to turn AP on when there is a ping alert
- PingTimer unit removed
- ping_output.cpp renamed to meter_output.cpp

2025-01-14 v.0.16 Debug level is now in AP Page
- Debug level is now in AP Page
- Ping Interval is now in the preferences

2025-01-14 v.0.15 APP_PREF_PING_IP_INDEX now saved and retrieved
- APP_PREF_PING_IP_INDEX now saved and retrieved
  It seems that this started working after shortening the flash memory key to
"ping_ip_index"
- #issue008 AP Boolean checkboxes are always unchecked

2025-01-14 v.0.14 AP Boolean field fixed
- #issue005 AP Page does not show boolean as a checkbox
- CurrIPIndex rep

2025-01-13 v.0.13 Serial.print replaced with Debug
- Serial.print replaced with Debug
- irdebug removed
- Done:#issue003 The sound needs to be played in async way

2025-01-13 v.0.12 WiFi Disconnection Alert
- WiFi disconnection from Ping
- WiFi Alert light and sound
- Millis Transition constants refactored

2025-01-13 v.0.11 Various fixes
- Serial.print to Debug
- prefs fixes

2025-01-12 v.0.10.0 AP Field Format is now union
- Use union to store AP Field Format definition

2025-01-12 v.0.9.0 Gemini-suggested refactoring
- Access Point
- App Preferences

2025-01-10 v.0.8.0 Ping LED
- Ping now lights up fading LED
- Server selection now lights up

2025-01-08 v.0.7.0
- Transitions are now named after triggers

2025-01-07 v.0.6.0
- Ping Alert to Ping is now timed transition
- Exceeding Max Ping Alert results in reboot
- Setting On/Off sound

2024-01-06 v.0.5.0
- New meter, correction removed
- Some debug reorg

2024-12-30 v.0.4.0
- Output correction with interpolation implemented (check slope)

2024-12-15 v.0.3.0
- Sound added, different melodies for different events.
- DNS server selection with sound

2024-12-14 v.0.2.0
- Ping Server change on Long Touch

2024-12-13 v.0.1.0
- Access Point generates Captive Portal page programmatically
- Long and Short Touch detection

*/

/* ToDo
- Implement state machine
- Make sure that we can reset() the TimedTransition
- Demo
- Backlight on/of
- MQTT
- LED color adjustments

*/
