
@'
# CO3009-261-CC-Group12

Course project for **Microprocessors - Microcontrollers (CO3009)**, Semester 1, 2026-2027.
Ho Chi Minh City University of Technology (HCMUT), VNU-HCM.

Board: **XNUCLEO-F103RB** (STM32F103RB)

## Group 12

| No. | Full name | Student ID | GitHub account |
|-----|-----------|------------|----------------|
| 1 | Luu Trong Hung | 2452419 | Hungluu1702 |


## Project description

A traffic light controller for a two-road intersection, running on a custom O(1) scheduler.

Features:
- **Automatic mode:** Red / Green / Amber lights of the two roads cycle alternately.
- **Manual mode:** change the light state with push buttons.
- **Timing adjustment mode:** change Red / Green / Amber durations with buttons, shown on the LCD.
- **Environment display:** read the DHT20 sensor and show temperature and humidity on the LCD.
- **Scheduler:** SCH_Add_Task, SCH_Delete_Task, SCH_Update (O(1)), SCH_Dispatch_Tasks, with one-shot task support.

## Hardware

| Device | Connection |
|--------|------------|
| Traffic light module 1 | D3 - D4 |
| Traffic light module 2 | D5 - D6 |
| Button module 1 | D7 - D8 |
| Button module 2 | D9 - D10 |
| DHT20 sensor + LCD | I2C slot (red) |

## Folder structure