#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdint.h>

/* Shared FSM state. All access belongs in main context. */
typedef enum {
  MODE_AUTOMATIC,
  MODE_MANUAL,
  MODE_TUNING,
  MODE_SAFE_FAULT
} SystemMode;

typedef enum {
  AUTO_RED1_GREEN2,
  AUTO_RED1_AMBER2,
  AUTO_GREEN1_RED2,
  AUTO_AMBER1_RED2
} AutomaticState;

typedef enum {
  MAN_ALL_RED,
  MAN_RED1_GREEN2,
  MAN_RED1_AMBER2,
  MAN_GREEN1_RED2,
  MAN_AMBER1_RED2
} ManualState;

typedef enum {
  TUNE_RED,
  TUNE_GREEN,
  TUNE_AMBER,
  TUNE_REVIEW
} TuningState;

typedef enum {
  TUNE_ERROR_NONE,
  TUNE_ERROR_LIMIT,
  TUNE_ERROR_RANGE,
  TUNE_ERROR_RELATION
} TuningError;

typedef struct {
  uint16_t red_s;
  uint16_t green_s;
  uint16_t amber_s;
} TrafficTiming;

/* Symmetric roads: valid iff all values are 1..99 AND red = green + amber.
 * Validate with a widened sum before committing all three values together.
 */
#define TRAFFIC_MIN_SECONDS       1U
#define TRAFFIC_MAX_SECONDS      99U
#define TRAFFIC_DEFAULT_RED_S     5U
#define TRAFFIC_DEFAULT_GREEN_S   3U
#define TRAFFIC_DEFAULT_AMBER_S   2U

#define BUTTON_MODE_INDEX        0U
#define BUTTON_SELECT_INDEX      1U
#define BUTTON_PLUS_INDEX        2U
#define BUTTON_MINUS_INDEX       3U
#define TIMER_AUTO_PHASE          0U

extern SystemMode system_mode;
extern AutomaticState automatic_state;
extern ManualState manual_state;
extern TuningState tuning_state;
extern TuningError tuning_error;
extern TrafficTiming active_timing;
extern TrafficTiming draft_timing;

#endif /* GLOBAL_H */

