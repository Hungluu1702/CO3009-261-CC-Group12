#include "software_timer.h"
#include "scheduler.h"

static uint32_t remaining_ticks[SOFTWARE_TIMER_COUNT];
uint8_t timer_flag[SOFTWARE_TIMER_COUNT];

void software_timer_init(void)
{
  for (uint8_t id = 0; id < SOFTWARE_TIMER_COUNT; id++)
  {
    remaining_ticks[id] = 0U;
    timer_flag[id] = 0U;
  }
}

void software_timer_run(void)
{
  for (uint8_t id = 0; id < SOFTWARE_TIMER_COUNT; id++)
  {
    if (remaining_ticks[id] > 0U)
    {
      remaining_ticks[id]--;
      if (remaining_ticks[id] == 0U)
      {
        timer_flag[id] = 1U;
      }
    }
  }
}

void setTimer(uint8_t id, uint32_t duration_ms)
{
  if (id >= SOFTWARE_TIMER_COUNT)
  {
    return;
  }

  /* Division first avoids overflowing for durations near UINT32_MAX. */
  remaining_ticks[id] = duration_ms / SCH_TICK_MS;
  if (duration_ms % SCH_TICK_MS != 0U)
  {
    remaining_ticks[id]++;
  }
  timer_flag[id] = (remaining_ticks[id] == 0U) ? 1U : 0U;
}

void cancelTimer(uint8_t id)
{
  if (id >= SOFTWARE_TIMER_COUNT)
  {
    return;
  }

  remaining_ticks[id] = 0U;
  timer_flag[id] = 0U;
}
