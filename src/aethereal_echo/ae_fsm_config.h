#ifndef AE_FSM_CONFIG_H
#define AE_FSM_CONFIG_H

#include "SimpleFSM.h"
#include "State.h"

extern SimpleFSM fsm;

#define FSM_RUN_MILLIS 100

// timed transition times
#define WIFI_CONNECTED_TO_PING_MILLIS 3 * 1000
#define AP_RUNNING_TO_REBOOT 3 * 60 * 1000
#define IP_SELECT_TO_WIFI_CONNECTED_MILLIS 30 * 1000
#define PING_SOUND_SELECT_TO_WIFI_CONNECTED_MILLIS 20 * 1000
#define ALERT_ENABLED_SELECT_TO_WIFI_CONNECTED_MILLIS 20 * 1000
#define WIFI_ALERT_TOGGLE_MILLIS 250
#define PING_ALERT_MILLIS 3 * 1000
#define WIFI_ALERT_TO_DISCONNECTED_MILLIS 20 * 1000
#define WIFI_DISCONNECTED_TO_REBOOT_MILLIS 10 * 1000

enum stateName {
    ST_BOOT,
    ST_WIFI_CONNECTED,
    ST_AP_RUNNING,
    ST_PING,
    ST_PING_ALERT,
    ST_IP_SELECT,
    ST_IP_NEXT,
    ST_PING_SOUND_SELECT,
    ST_PING_SOUND_TOGGLE,
    ST_ALERT_ENABLED_SELECT,
    ST_ALERT_ENABLED_TOGGLE,
    ST_WIFI_ALERT,
    ST_WIFI_ALERT_TOGGLE,
    ST_WIFI_DISCONNECTED,
    ST_REBOOT,
    NUM_OF_STATES
};

enum triggers {
    tr_wifi_connected = 1,
    tr_wifi_not_connected,
    tr_wifi_lost,
    tr_wifi_alert_end,
    tr_wifi_alert_toggle_end,
    tr_wifi_reconnected,
    tr_ap_config_ok,
    tr_ap_config_err,
    tr_ping_timeout,
    tr_ping_ok,
    tr_ping_alert_end,
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
    TTR_PING_ALERT_TO_PING,
    TTR_IP_SELECT_TO_WIFI_CONNECTED,
    TTR_PING_SOUND_SELECT_TO_WIFI_CONNECTED,
    TTR_ALERT_ENABLED_SELECT_TO_WIFI_CONNECTED,
    TTR_WIFI_ALERT_TO_WIFI_ALERT_TOGGLE,
    TTR_WIFI_ALERT_TO_WIFI_DISCONNECTED,
    TTR_WIFI_DISCONNECTED_TO_REBOOT,
    // TTR_PING_SOUND_TOGGLE_TO_PING_SOUND_SELECT,
    NUM_OF_TTR
};

extern TimedTransition timedTransitions[NUM_OF_TTR];

extern void aeFsmSetup();
extern void aeFsmLoop();
extern String aeShowGraph();

#endif // AE_FSM_CONFIG_H

