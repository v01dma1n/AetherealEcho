#define ESP32DEBUGGING
#include <ESP32Logger.h>

#include "access_point.h"
#include "ae_fsm_config.h"
#include "ae_fsm_execute.h"
#include "aethereal_echo.h"
#include "app_pref.h"
#include "melody.h"
#include "version.h"
#include "wifi_connect.h"

SimpleFSM fsm;

// State definitions
State s[NUM_OF_STATES] = {
    State("boot", on_enter_boot),
    State("wifi_connected", on_enter_wifi_connected),
    State("ap_running", on_enter_ap_running),
    State("ping", on_enter_ping),
    State("ping_alert", on_enter_ping_alert, on_state_ping_alert),
    State("ip_select", on_enter_ip_select, NULL, on_exit_ip_select),
    State("ip_next", on_enter_ip_next),
    State("ping_sound_select", on_enter_ping_sound_select),
    State("ping_sound_toggle", on_enter_ping_sound_toggle),
    State("alert_enabled_select", on_enter_alert_enabled_select),
    State("alert_enabled_toggle", on_enter_alert_enabled_toggle),
    State("wifi_alert", on_enter_wifi_alert),
    State("wifi_alert_toggle", on_enter_wifi_alert_toggle),
    State("wifi_disconnected", on_enter_wifi_disconnected),
    State("reboot", on_enter_reboot)
};

// Transition definitions
Transition transitions[] = {
    Transition(&s[ST_BOOT], &s[ST_WIFI_CONNECTED], tr_wifi_connected, NULL,"tr_wifi_connected"),
    Transition(&s[ST_BOOT], &s[ST_AP_RUNNING], tr_wifi_not_connected, NULL,"tr_wifi_not_connected"),
    Transition(&s[ST_AP_RUNNING], &s[ST_REBOOT], tr_ap_config_ok, NULL,"tr_ap_config_ok"),
    Transition(&s[ST_AP_RUNNING], &s[ST_REBOOT], tr_ap_long_press, NULL,"tr_ap_long_press"),
    Transition(&s[ST_AP_RUNNING], &s[ST_AP_RUNNING], tr_ap_config_err, NULL,"tr_ap_config_err"),
    Transition(&s[ST_WIFI_CONNECTED], &s[ST_AP_RUNNING], tr_ap_long_press, NULL,"tr_ap_long_press"),
    Transition(&s[ST_PING], &s[ST_PING_ALERT], tr_ping_timeout, NULL,"tr_ping_timeout"),
    Transition(&s[ST_PING], &s[ST_WIFI_CONNECTED], tr_ping_ok, NULL,"tr_ping_ok"),
    Transition(&s[ST_PING], &s[ST_WIFI_ALERT], tr_wifi_lost, NULL,"tr_wifi_lost"),
    Transition(&s[ST_PING_ALERT], &s[ST_WIFI_CONNECTED], tr_ping_alert_end, NULL, "tr_ping_alert_end"),
    Transition(&s[ST_PING_ALERT], &s[ST_REBOOT], tr_ping_timeout_max, NULL,"tr_ping_timeout_max"),
    Transition(&s[ST_PING_ALERT], &s[ST_IP_SELECT], tr_ip_select_long_press, NULL, "tr_ip_select_long_press"),
    Transition(&s[ST_PING_ALERT], &s[ST_AP_RUNNING], tr_ap_long_press, NULL,"tr_ap_long_press"),
    // select IP for pinging
    Transition(&s[ST_WIFI_CONNECTED], &s[ST_IP_SELECT], tr_ip_select_long_press, NULL, "tr_ip_select_long_press"),
    Transition(&s[ST_IP_SELECT], &s[ST_IP_NEXT], tr_ip_select_short_press, NULL,"tr_ip_select_short_press"),
    Transition(&s[ST_IP_SELECT], &s[ST_WIFI_CONNECTED], tr_ip_select_long_press, NULL, "tr_ip_select_long_press"),
    Transition(&s[ST_IP_NEXT], &s[ST_IP_SELECT], tr_ip_select_done, NULL,"tr_ip_select_done"),
    // ping sound on or off
    Transition(&s[ST_WIFI_CONNECTED], &s[ST_PING_SOUND_SELECT], tr_ping_sound_long_press, NULL, "tr_ping_sound_long_press"),
    Transition(&s[ST_PING_SOUND_SELECT], &s[ST_PING_SOUND_TOGGLE],tr_ping_sound_short_press, NULL, "tr_ping_sound_short_press"),
    Transition(&s[ST_PING_SOUND_SELECT], &s[ST_WIFI_CONNECTED],tr_ping_sound_long_press, NULL, "tr_ping_sound_long_press"),
    Transition(&s[ST_PING_SOUND_TOGGLE], &s[ST_PING_SOUND_SELECT],tr_ping_sound_select_done, NULL, "tr_ping_sound_select_done"),
    // alert enabled on or off
    Transition(&s[ST_WIFI_CONNECTED], &s[ST_ALERT_ENABLED_SELECT], tr_alert_enabled_long_press, NULL,"tr_alert_enabled_long_press"),
    Transition(&s[ST_ALERT_ENABLED_SELECT], &s[ST_ALERT_ENABLED_TOGGLE],tr_alert_enabled_short_press, NULL,"tr_alert_enabled_short_press"),
    Transition(&s[ST_ALERT_ENABLED_SELECT], &s[ST_WIFI_CONNECTED],tr_alert_enabled_long_press, NULL,"tr_alert_enabled_long_press"),
    Transition(&s[ST_ALERT_ENABLED_TOGGLE], &s[ST_ALERT_ENABLED_SELECT],tr_alert_enabled_select_done, NULL,"tr_alert_enabled_select_done"),
    Transition(&s[ST_WIFI_CONNECTED], &s[ST_WIFI_ALERT], tr_wifi_lost, NULL,"tr_wifi_lost"),
    Transition(&s[ST_WIFI_ALERT], &s[ST_WIFI_DISCONNECTED], tr_wifi_alert_end, NULL,"tr_wifi_alert_end"),
    Transition(&s[ST_WIFI_ALERT_TOGGLE], &s[ST_WIFI_ALERT], tr_wifi_alert_toggle_end,NULL, "tr_wifi_alert_toggle_end"),
    Transition(&s[ST_WIFI_DISCONNECTED], &s[ST_WIFI_CONNECTED],tr_wifi_reconnected, NULL, "tr_wifi_reconnected"),
};

// Timed transition definitions
TimedTransition timedTransitions[NUM_OF_TTR] = {
    TimedTransition(&s[ST_AP_RUNNING], &s[ST_REBOOT], AP_RUNNING_TO_REBOOT), // Acess Point Timeout
    TimedTransition(&s[ST_WIFI_CONNECTED], &s[ST_PING], WIFI_CONNECTED_TO_PING_MILLIS), // how frequently to ping
    TimedTransition(&s[ST_PING_ALERT], &s[ST_PING], PING_ALERT_MILLIS), // how long ping alert lastas
    TimedTransition(&s[ST_IP_SELECT], &s[ST_WIFI_CONNECTED], IP_SELECT_TO_WIFI_CONNECTED_MILLIS), // IP selection mode timeout
    TimedTransition(&s[ST_PING_SOUND_SELECT], &s[ST_WIFI_CONNECTED], PING_SOUND_SELECT_TO_WIFI_CONNECTED_MILLIS), // ping sound enabled/disabbled timeout
    TimedTransition(&s[ST_ALERT_ENABLED_SELECT], &s[ST_WIFI_CONNECTED], ALERT_ENABLED_SELECT_TO_WIFI_CONNECTED_MILLIS), // ping alert enabled/disabbled timeout
    TimedTransition(&s[ST_WIFI_ALERT], &s[ST_WIFI_ALERT_TOGGLE],WIFI_ALERT_TOGGLE_MILLIS),
    TimedTransition(&s[ST_WIFI_ALERT], &s[ST_WIFI_DISCONNECTED],WIFI_ALERT_TO_DISCONNECTED_MILLIS),
    TimedTransition(&s[ST_WIFI_DISCONNECTED], &s[ST_REBOOT],WIFI_DISCONNECTED_TO_REBOOT_MILLIS)
};

int numTransitions = sizeof(transitions) / sizeof(Transition);
int numTimedTransitions = sizeof(timedTransitions) / sizeof(TimedTransition);

void aeFsmSetup() {

  // tweak the timed transitions based on the configuration
  timedTransitions[TTR_WIFI_CONNECTED_TO_PING].setup(&s[ST_WIFI_CONNECTED], &s[ST_PING],
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
};