#ifndef AE_STATE_MACHINE_H
#define AE_STATE_MACHINE_H

#include "SimpleFSM.h"
#include "State.h"

extern SimpleFSM fsm;

#define FSM_RUN_MILLIS 100

// timed transition times

// default ping interval
#define WIFI_CONNECTED_TO_PING_MILLIS 3 * 1000

// how long the AP can run before reboot
#define AP_RUNNING_TO_REBOOT 5 * 60 * 1000

// how quickly the opeation should resume after changes
#define IP_SELECT_TO_WIFI_CONNECTED_MILLIS 20 * 1000
#define PING_SOUND_SELECT_TO_WIFI_CONNECTED_MILLIS 20 * 1000
#define ALERT_ENABLED_SELECT_TO_WIFI_CONNECTED_MILLIS 20 * 1000

// interval between pings after a timeout
#define PING_TIMEOUT_MILLIS 3 * 1000

// ping alert timings
#define PING_ALERT_SWITCH_MILLIS 250
#define PING_ALERT_REBOOT_MILLIS 10 * 1000

// wifi alert timings
#define WIFI_ALERT_TO_DISCONNECTED_MILLIS 20 * 1000
#define WIFI_DISCONNECTED_TO_REBOOT_MILLIS 10 * 1000
#define WIFI_ALERT_SWITCH_MILLIS 250

enum stateName {
  ST_BOOT,
  ST_WIFI_CONNECTED,
  ST_AP_RUNNING,
  ST_PING,
  ST_PING_TIMEOUT,
  ST_PING_ALERT,
  ST_PING_ALERT_SWITCH,
  ST_IP_SELECT,
  ST_IP_NEXT,
  ST_PING_SOUND_SELECT,
  ST_PING_SOUND_SWITCH,
  ST_ALERT_ENABLED_SELECT,
  ST_ALERT_ENABLED_SWITCH,
  ST_WIFI_ALERT,
  ST_WIFI_ALERT_SWITCH,
  ST_WIFI_DISCONNECTED,
  ST_REBOOT,
  NUM_OF_STATES
};

enum triggers {
  tr_wifi_connected = 1,
  tr_wifi_not_connected,
  tr_wifi_lost,
  tr_wifi_alert_end,
  tr_wifi_alert_switch_end,
  tr_wifi_reconnected,

  tr_ap_config_ok,
  tr_ap_config_err,

  tr_ping_ok,
  tr_ping_timeout,
  tr_ping_alert,
  tr_ping_alert_end,
  tr_ping_alert_switch_end,
  tr_ping_timeout_max,

  tr_ap_long_press,
  tr_ap_short_press,

  tr_ip_select_short_press,
  tr_ip_select_long_press,
  tr_ip_select_done,

  tr_ping_sound_short_press,
  tr_ping_sound_long_press,
  tr_ping_sound_select_done,

  tr_alert_enabled_short_press,
  tr_alert_enabled_long_press,
  tr_alert_enabled_select_done
};

enum TimedTransitionName {
  TTR_AP_RUNNING_TO_RESET,
  TTR_WIFI_CONNECTED_TO_PING,

  TTR_PING_TIMEOUT_TO_PING,
  TTR_PING_ALERT_SWITCH_TO_PING_ALERT,
  TTR_PING_ALERT_SWITCH_TO_REBOOT,

  TTR_IP_SELECT_TO_WIFI_CONNECTED,

  TTR_PING_SOUND_SELECT_TO_WIFI_CONNECTED,
  TTR_ALERT_ENABLED_SELECT_TO_WIFI_CONNECTED,

  TTR_WIFI_ALERT_TO_WIFI_ALERT_SWITCH,
  TTR_WIFI_ALERT_TO_WIFI_DISCONNECTED,

  TTR_WIFI_DISCONNECTED_TO_REBOOT,
  NUM_OF_TTR
};

extern void aeFsmSetup();
extern void aeFsmLoop();

extern String aeShowGraph();

extern void on_enter_boot();
extern void on_enter_wifi_connected();
extern void on_enter_ap_running();
extern void on_enter_ping();
extern void on_enter_ping_timeout();
extern void on_enter_ping_alert();
extern void on_enter_ping_alert_switch();
extern void on_enter_ip_select();
extern void on_enter_ip_next();
extern void on_enter_ping_sound_select();
extern void on_enter_ping_sound_switch();
extern void on_enter_alert_enabled_select();
extern void on_enter_alert_enabled_switch();
extern void on_enter_wifi_alert();
extern void on_enter_wifi_alert_switch();
extern void on_enter_wifi_disconnected();
extern void on_enter_reboot();

extern TimedTransition timedTransitions[NUM_OF_TTR];

#endif // ACCESS_POINT
