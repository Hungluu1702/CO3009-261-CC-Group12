#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

typedef enum {
  ROAD_1 = 1,
  ROAD_2 = 2
} TrafficRoad;

typedef enum {
  TRAFFIC_OFF,
  TRAFFIC_RED,
  TRAFFIC_GREEN,
  TRAFFIC_AMBER
} TrafficColor;

/* Call after all peripheral initialization/remaps: releases JTAG pins,
 * configures the four GPIO outputs and sets both roads to RED. Keeps SWD. */
void traffic_init(void);

/* Set one road without changing the other road or unrelated GPIO pins.
 * Invalid road/color values are ignored. Call from main/scheduler tasks.
 * The FSM owns timing and must set the outgoing road RED before giving
 * the other road GREEN. This driver does not enforce cross-road policy. */
void traffic_set(TrafficRoad road, TrafficColor color);

/* Set both roads in one GPIO write. Only RED/RED, RED/GREEN,
 * RED/AMBER, GREEN/RED and AMBER/RED are accepted.
 * An invalid pair commands both RED and returns 0; success returns 1.
 * Call after traffic_init(), from main/scheduler context only. */
int traffic_set_pair(TrafficColor road_1, TrafficColor road_2);

#endif
