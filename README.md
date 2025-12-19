# RX-8025RTC Library for Arduino

An optimized, easy-to-use library for the **Epson RX-8025SA/NB** Real-Time Clock (RTC) module.

This library provides full control over timekeeping, calendar management, and advanced features like dual interrupts and periodic timers. It is specifically designed to handle the unique register structure and N-ch open-drain interrupt outputs of the RX-8025.

## Features

* **Full Timekeeping**: 24-hour clock, seconds, minutes, hours, day, month, year, and day of the week.
* **Dual Alarms**:
* **Alarm D**: Daily alarm triggering `/INTA`.
* **Alarm W**: Weekly alarm (selectable days) triggering `/INTB`.
* **Periodic Timer**: Supports various interrupt cycles (1Hz, 1min, 1hour, etc.) on `/INTA`.

## Installation

1. Download this repository as a ZIP file.
2. In the Arduino IDE, go to **Sketch** -> **Include Library** -> **Add .ZIP Library...**.
3. Select the downloaded ZIP file.

## Wiring

| RX-8025 Pin | Arduino Uno Pin | Description |
| --- | --- | --- |
| VCC | 5V / 3.3V | Power Supply |
| GND | GND | Ground |
| SDA | A4 (SDA) | I2C Data (Needs Pull-up) |
| SCL | A5 (SCL) | I2C Clock (Needs Pull-up) |
| /INTA | D2 | Interrupt A (Alarm D / Timer) |
| /INTB | D3 | Interrupt B (Alarm W) |

*Note: `/INTA` and `/INTB` are N-ch open-drain. Use `INPUT_PULLUP` in your code.*

## Quick Start

### Basic Time Setting

```cpp
#include <Wire.h>
#include <RX-8025RTC.h>

RX8025RTC rtc(Wire);

void setup() {
    rtc.begin();
    rtc.setTime(14, 30, 0); // 14:30:00
    rtc.setDate(25, 12, 19, W_FRI); // Dec 19th, 2025 (Friday)
}

```

### Sleep with Interrupts

Check the `examples/SleepAlarm/SleepAlarm.ino` for a complete guide on how to wake up an Arduino Uno from Power-down mode.

## Library API

* `setTime(h, m, s)` / `getTime(h, m, s)`
* `setDate(y, m, d, wday)` / `getDate(y, m, d, wday)`
* `setAlarmD(h, m)` / `enableAlarmD(true)`
* `setAlarmW(h, m, weekMask)` / `enableAlarmW(true)`
* `setCycleTimer(period)` / `enableCycleTimer(true)`
* `voltageLow()` : Returns true if backup battery is low.

## License

This library is released under the MIT License.
