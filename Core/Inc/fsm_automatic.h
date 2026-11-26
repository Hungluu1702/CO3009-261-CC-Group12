#ifndef FSM_AUTOMATIC_H
#define FSM_AUTOMATIC_H
/* Main context only. Initialize after drivers and software timers.
 * Run every 10 ms after software_timer_run and button_read.
 * Manual mode is handed off at MAN_ALL_RED; its FSM is separate.
 */
void fsm_automatic_init(void);
void fsm_automatic_run(void);
#endif
