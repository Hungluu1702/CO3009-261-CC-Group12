#include "traffic_light.h"
#include "main.h"

/* This board routes all four signals to GPIOB. Keep the single-write
 * guarantee explicit if the CubeMX pin mapping changes. */
#define TRAFFIC_PINS (TL1_A_Pin | TL1_B_Pin | TL2_A_Pin | TL2_B_Pin)

static int color_pins(TrafficColor color, uint32_t pin_a,
                      uint32_t pin_b, uint32_t *set_pins)
{
  /* User-measured truth table: AB=00 OFF, 01 AMBER, 10 GREEN, 11 RED. */
  switch (color)
  {
    case TRAFFIC_OFF:   *set_pins = 0U;            return 1;
    case TRAFFIC_AMBER: *set_pins = pin_b;         return 1;
    case TRAFFIC_GREEN: *set_pins = pin_a;         return 1;
    case TRAFFIC_RED:   *set_pins = pin_a | pin_b; return 1;
    default: return 0;
  }
}

static void write_pins(GPIO_TypeDef *port, uint32_t mask, uint32_t set_pins)
{
  port->BSRR = set_pins | ((mask & ~set_pins) << 16U);
}

void traffic_init(void)
{
  GPIO_InitTypeDef gpio = {0};

  /* Run after peripheral remaps (notably I2C1). PB3/PB4 belong to JTAG
   * after reset; retain SWD on PA13/PA14 for the debugger. */
  __HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_AFIO_REMAP_SWJ_NOJTAG();

  /* Preload the output latches before enabling push-pull outputs. */
  (void)traffic_set_pair(TRAFFIC_RED, TRAFFIC_RED);
  gpio.Pin = TRAFFIC_PINS;
  gpio.Mode = GPIO_MODE_OUTPUT_PP;
  gpio.Pull = GPIO_NOPULL;
  gpio.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &gpio);
}

void traffic_set(TrafficRoad road, TrafficColor color)
{
  uint32_t pin_a;
  uint32_t pin_b;
  GPIO_TypeDef *port;

  /* Both signals of each module are on GPIOB in this project's pinout. */
  switch (road)
  {
    case ROAD_1:
      port = TL1_A_GPIO_Port;
      pin_a = TL1_A_Pin;
      pin_b = TL1_B_Pin;
      break;
    case ROAD_2:
      port = TL2_A_GPIO_Port;
      pin_a = TL2_A_Pin;
      pin_b = TL2_B_Pin;
      break;
    default:
      return;
  }

  uint32_t set_pins;
  if (!color_pins(color, pin_a, pin_b, &set_pins))
  {
    return;
  }

  /* One BSRR write updates A and B together, avoiding the intermediate
   * color produced by two sequential GPIO writes. Other pins are untouched.
   * BSRR bits 0..15 set outputs; bits 16..31 reset outputs. */
  write_pins(port, pin_a | pin_b, set_pins);
}

int traffic_set_pair(TrafficColor road_1, TrafficColor road_2)
{
  uint32_t pins_1;
  uint32_t pins_2;
  int valid = (road_1 == TRAFFIC_RED &&
               (road_2 == TRAFFIC_RED || road_2 == TRAFFIC_GREEN ||
                road_2 == TRAFFIC_AMBER)) ||
              (road_2 == TRAFFIC_RED &&
               (road_1 == TRAFFIC_GREEN || road_1 == TRAFFIC_AMBER));

  if (!valid)
  {
    write_pins(GPIOB, TRAFFIC_PINS, TRAFFIC_PINS);
    return 0;
  }

  (void)color_pins(road_1, TL1_A_Pin, TL1_B_Pin, &pins_1);
  (void)color_pins(road_2, TL2_A_Pin, TL2_B_Pin, &pins_2);
  write_pins(GPIOB, TRAFFIC_PINS, pins_1 | pins_2);
  return 1;
}
