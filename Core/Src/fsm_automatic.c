#include "fsm_automatic.h"
#include "global.h"
#include "traffic_light.h"
#include "software_timer.h"
#include "button.h"

static void stop_phase(void)
{
    cancelTimer(TIMER_AUTO_PHASE);
    traffic_set(ROAD_1, TRAFFIC_RED);
    traffic_set(ROAD_2, TRAFFIC_RED);
    button_clear_all();
}

static int timing_valid(void)
{
    return active_timing.red_s >= 1U && active_timing.red_s <= 99U &&
           active_timing.green_s >= 1U && active_timing.green_s <= 99U &&
           active_timing.amber_s >= 1U && active_timing.amber_s <= 99U &&
           active_timing.red_s == (uint32_t)active_timing.green_s + active_timing.amber_s;
}

static void enter_phase(AutomaticState state)
{
    automatic_state = state;
    switch (state) {
    case AUTO_RED1_GREEN2:
        traffic_set(ROAD_1, TRAFFIC_RED);
        traffic_set(ROAD_2, TRAFFIC_GREEN);
        setTimer(TIMER_AUTO_PHASE, active_timing.green_s * 1000U);
        break;
    case AUTO_RED1_AMBER2:
        traffic_set(ROAD_1, TRAFFIC_RED);
        traffic_set(ROAD_2, TRAFFIC_AMBER);
        setTimer(TIMER_AUTO_PHASE, active_timing.amber_s * 1000U);
        break;
    case AUTO_GREEN1_RED2:
        traffic_set(ROAD_2, TRAFFIC_RED);
        traffic_set(ROAD_1, TRAFFIC_GREEN);
        setTimer(TIMER_AUTO_PHASE, active_timing.green_s * 1000U);
        break;
    case AUTO_AMBER1_RED2:
        traffic_set(ROAD_2, TRAFFIC_RED);
        traffic_set(ROAD_1, TRAFFIC_AMBER);
        setTimer(TIMER_AUTO_PHASE, active_timing.amber_s * 1000U);
        break;
    }
}

void fsm_automatic_init(void)
{
    stop_phase();
    if (!timing_valid()) {
        system_mode = MODE_SAFE_FAULT;
        draft_timing = active_timing;
        return;
    }
    system_mode = MODE_AUTOMATIC;
    enter_phase(AUTO_RED1_GREEN2);
}

void fsm_automatic_run(void)
{
    if (system_mode == MODE_SAFE_FAULT) {
        if (isButtonPressed(BUTTON_MODE_INDEX)) {
            active_timing = (TrafficTiming){TRAFFIC_DEFAULT_RED_S,
                TRAFFIC_DEFAULT_GREEN_S, TRAFFIC_DEFAULT_AMBER_S};
            draft_timing = active_timing;
            fsm_automatic_init();
        }
        return;
    }
    if (system_mode == MODE_MANUAL || system_mode == MODE_TUNING) return;
    if (system_mode != MODE_AUTOMATIC || !timing_valid() ||
        automatic_state < AUTO_RED1_GREEN2 || automatic_state > AUTO_AMBER1_RED2) {
        stop_phase();
        draft_timing = active_timing;
        tuning_error = TUNE_ERROR_NONE;
        system_mode = MODE_SAFE_FAULT;
        return;
    }
    /* A mode press wins even when the phase expires on this tick. */
    if (isButtonPressed(BUTTON_MODE_INDEX)) {
        stop_phase();
        manual_state = MAN_ALL_RED;
        system_mode = MODE_MANUAL;
        return;
    }
    if (!timer_flag[TIMER_AUTO_PHASE]) return;
    switch (automatic_state) {
    case AUTO_RED1_GREEN2: enter_phase(AUTO_RED1_AMBER2); break;
    case AUTO_RED1_AMBER2: enter_phase(AUTO_GREEN1_RED2); break;
    case AUTO_GREEN1_RED2: enter_phase(AUTO_AMBER1_RED2); break;
    case AUTO_AMBER1_RED2: enter_phase(AUTO_RED1_GREEN2); break;
    }
}
