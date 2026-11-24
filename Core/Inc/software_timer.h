#ifndef SOFTWARE_TIMER_H
#define SOFTWARE_TIMER_H

#include <stdint.h>

#define SOFTWARE_TIMER_COUNT 4U

/* IDs are 0..3. A flag stays set after expiry until setTimer(),
 * cancelTimer(), or software_timer_init() clears it. */
extern uint8_t timer_flag[SOFTWARE_TIMER_COUNT];

/* All functions and flag reads belong in main context, including scheduler
 * callbacks. No software timer API is called from an interrupt. */
void software_timer_init(void);
/* Register once with delay=1, period=1 on the 10 ms scheduler. */
void software_timer_run(void);
/* Duration is milliseconds, rounded UP to scheduler ticks without overflow.
 * Zero expires immediately. Setting a running timer restarts it.
 * Resolution is 10 ms; expiry also depends on the update task's phase/latency.
 * Invalid IDs are ignored. */
void setTimer(uint8_t id, uint32_t duration_ms);
/* Stop the selected timer and clear its flag. Invalid IDs are ignored. */
void cancelTimer(uint8_t id);

#endif
