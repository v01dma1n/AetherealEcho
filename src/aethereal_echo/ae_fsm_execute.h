#ifndef AE_FSM_EXECUTE_H
#define AE_FSM_EXECUTE_H

// Function prototypes for state handlers
void fsm_tick();
void on_enter_boot();
void on_enter_wifi_connected();
void on_enter_ap_running();
void on_enter_ping();
void on_enter_ping_alert();
void on_state_ping_alert();
void on_enter_ip_select();
void on_exit_ip_select();
void on_enter_ip_next();
void on_enter_ping_sound_select();
void on_enter_ping_sound_switch();
void on_enter_alert_enabled_select();
void on_enter_alert_enabled_switch();
void on_enter_wifi_alert();
void on_enter_wifi_alert_switch();
void on_enter_wifi_disconnected();
void on_enter_reboot();


#endif // AE_FSM_EXECUTE_H