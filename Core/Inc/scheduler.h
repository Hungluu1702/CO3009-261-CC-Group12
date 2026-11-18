#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

#define SCH_MAX_TASKS 16U
#define SCH_TICK_MS 10U
#define SCH_INVALID_TASK_ID 0U

typedef void (*SCH_TaskFunction_t)(void);

/* Main-context API; delays/periods are ticks (one tick = 10 ms).
 * delay=0: ready immediately; period=0: run once.
 * Add returns a stable nonzero ID, or 0 for NULL/full/ID exhaustion.
 * Delete returns 1 if found, 0 otherwise. Callbacks must not block.
 */
void SCH_Init(void);
uint32_t SCH_Add_Task(SCH_TaskFunction_t task, uint32_t delay, uint32_t period);
uint8_t SCH_Delete_Task(uint32_t task_id);
void SCH_Dispatch_Tasks(void);
/* Call only from the 10 ms timer ISR. O(1), never executes a callback. */
void SCH_Update(void);

#endif
