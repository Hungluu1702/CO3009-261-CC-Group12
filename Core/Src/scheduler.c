#include "scheduler.h"
#include "stm32f1xx_hal.h"

typedef struct {
  SCH_TaskFunction_t function;
  uint32_t delay; /* Relative to preceding task. */
  uint32_t period;
  uint32_t id;
} SCH_Task;

static volatile SCH_Task tasks[SCH_MAX_TASKS];
static volatile uint32_t count;
static volatile uint32_t ticks;
static volatile uint32_t accounted_ticks;
static uint32_t next_id;

static uint32_t lock(void)
{
  uint32_t state = __get_PRIMASK();
  __disable_irq();
  return state;
}

/* Preserve elapsed time even when an overdue head waits for dispatch.
 * Main context settles the entire queue; the ISR touches only its head.
 * Requires at least one main-context call per uint32_t tick wrap (~497 days).
 */
static void settle(void)
{
  uint32_t elapsed = ticks - accounted_ticks;
  for (uint32_t i = 0; i < count && elapsed != 0; i++) {
    uint32_t consumed = tasks[i].delay < elapsed ? tasks[i].delay : elapsed;
    tasks[i].delay -= consumed;
    elapsed -= consumed;
  }
  accounted_ticks = ticks;
}

/* Caller holds interrupt lock. */
static void insert(SCH_Task task)
{
  uint32_t i = 0;
  while (i < count && task.delay >= tasks[i].delay) {
    task.delay -= tasks[i].delay;
    i++;
  }
  for (uint32_t j = count; j > i; j--) tasks[j] = tasks[j - 1];
  if (i < count) tasks[i + 1].delay -= task.delay;
  tasks[i] = task;
  count++;
}

static void remove_at(uint32_t index)
{
  if (index + 1 < count) tasks[index + 1].delay += tasks[index].delay;
  for (uint32_t j = index; j + 1 < count; j++) tasks[j] = tasks[j + 1];
  count--;
}

void SCH_Init(void)
{
  uint32_t state = lock();
  count = 0;
  ticks = 0;
  accounted_ticks = 0;
  next_id = 1;
  __set_PRIMASK(state);
}

uint32_t SCH_Add_Task(SCH_TaskFunction_t function, uint32_t delay, uint32_t period)
{
  uint32_t state = lock();
  uint32_t id = SCH_INVALID_TASK_ID;
  settle();
  if (function != 0 && count < SCH_MAX_TASKS && next_id != 0) {
    SCH_Task task = {function, delay, period, next_id++};
    id = task.id;
    insert(task);
  }
  __set_PRIMASK(state);
  return id;
}

uint8_t SCH_Delete_Task(uint32_t task_id)
{
  uint32_t state = lock();
  uint8_t found = 0;
  settle();
  for (uint32_t i = 0; i < count; i++) {
    if (tasks[i].id == task_id) {
      remove_at(i);
      found = 1;
      break;
    }
  }
  __set_PRIMASK(state);
  return found;
}

void SCH_Update(void)
{
  ticks++;
  if (count == 0) {
    accounted_ticks = ticks;
  } else {
    uint32_t elapsed = ticks - accounted_ticks;
    uint32_t consumed = tasks[0].delay < elapsed ? tasks[0].delay : elapsed;
    tasks[0].delay -= consumed;
    accounted_ticks += consumed;
  }
}

void SCH_Dispatch_Tasks(void)
{
  /* Bound each pass, including callbacks which add immediate tasks. */
  for (uint32_t n = 0; n < SCH_MAX_TASKS; n++) {
    uint32_t state = lock();
    settle();
    if (count == 0 || tasks[0].delay != 0) {
      __set_PRIMASK(state);
      return;
    }
    SCH_Task task = tasks[0];
    remove_at(0);
    /* Reinsert before callback: stable ID, self-delete supported.
     * Missed periods are coalesced; next release is period ticks from now.
     */
    if (task.period != 0) {
      task.delay = task.period;
      insert(task);
    }
    __set_PRIMASK(state);
    task.function();
  }
}
