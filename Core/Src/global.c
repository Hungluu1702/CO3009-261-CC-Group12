#include "global.h"
SystemMode system_mode = MODE_AUTOMATIC;
AutomaticState automatic_state = AUTO_RED1_GREEN2;
ManualState manual_state = MAN_ALL_RED;
TuningState tuning_state = TUNE_RED;
TuningError tuning_error = TUNE_ERROR_NONE;
TrafficTiming active_timing = {5U, 3U, 2U};
TrafficTiming draft_timing = {5U, 3U, 2U};
