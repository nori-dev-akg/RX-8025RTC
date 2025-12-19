/**
 * @file RX-8025RTC.h
 * @brief Arduino/PlatformIO library for Epson RX-8025SA/NB RTC.
 * @author nori-dev-akg
 * @license MIT
 * * This library provides full control over the RX-8025 RTC, including
 * Time, Calendar, Two Alarms (Weekly/Daily), and Periodic Timer.
 */

#pragma once
#include <Arduino.h>
#include <Wire.h>

/**
 * @brief Register Address Map for RX-8025.
 * Mode 0: Time/Calendar/Alarm
 * Mode 1: Extension/Digital Offset
 */
enum RX8025_Reg {
    // Mode 0
    REG_SEC         = 0x00, REG_MIN      = 0x01, REG_HOUR     = 0x02, REG_WEEK     = 0x03,
    REG_DAY         = 0x04, REG_MONTH    = 0x05, REG_YEAR     = 0x06, REG_ADJ      = 0x07,
    REG_ALM_W_MIN   = 0x08, REG_ALM_W_HOUR   = 0x09, REG_ALM_W_WEEK   = 0x0A,
    REG_ALM_D_MIN   = 0x0B, REG_ALM_D_HOUR   = 0x0C,
    REG_CTRL1       = 0x0E, REG_CTRL2        = 0x0F,
    // Mode 1
    REG_EXT_ADJ     = 0x10, REG_EXT_CTRL1    = 0x1E, REG_EXT_CTRL2    = 0x1F
};

enum Weekday {
    W_SUN = 0, W_MON = 1, W_TUE = 2, W_WED = 3,
    W_THU = 4, W_FRI = 5, W_SAT = 6
};


/**
 * @brief Bitmask for Alarm W (Weekly Alarm).
 */
enum WeekdayMask {
    BIT_SUN  = 0x01, BIT_MON  = 0x02, BIT_TUE  = 0x04, BIT_WED  = 0x08,
    BIT_THU  = 0x10, BIT_FRI  = 0x20, BIT_SAT  = 0x40, BIT_ALL  = 0x7F
};


class RX8025RTC {
public:
    /**
     * @brief Periodic interrupt cycle selection (CT2, CT1, CT0).
     */
    enum CyclePeriod {
        CYCLE_OFF      = 0x00, // Hi-z (OFF)
        CYCLE_FIXED_L  = 0x01, // "L" Fixed output
        CYCLE_2HZ_P    = 0x02, // 2Hz Pulse (Duty 50%)
        CYCLE_1HZ_P    = 0x03, // 1Hz Pulse (Duty 50%)
        CYCLE_1S_L     = 0x04, // 1sec Level mode
        CYCLE_1MIN_L   = 0x05, // 1min Level mode
        CYCLE_1H_L     = 0x06, // 1hour Level mode
        CYCLE_1MONTH_L = 0x07  // 1month Level mode
    };


    RX8025RTC(TwoWire &wire = Wire, uint8_t i2cAddr = 0x32);
    bool begin();

    // Time & Date
    bool setTime(uint8_t hour, uint8_t min, uint8_t sec);
    bool getTime(uint8_t &hour, uint8_t &min, uint8_t &sec);
    bool setDate(uint8_t year, uint8_t month, uint8_t day, Weekday wday);
    bool getDate(uint8_t &year, uint8_t &month, uint8_t &day, Weekday &wday);

    // Alarm D (/INTA)
    bool setAlarmD(uint8_t hour, uint8_t min);
    bool enableAlarmD(bool enable = true);
    bool alarmDFlag();
    bool clearAlarmDFlag();

    // Alarm W (/INTB)
    bool setAlarmW(uint8_t hour, uint8_t min, WeekdayMask weekMask = BIT_ALL);
    bool enableAlarmW(bool enable = true);
    bool alarmWFlag();
    bool clearAlarmWFlag();

    // Periodic interrupt (/INTA)
    bool setCycleTimer(CyclePeriod period);
    bool enableCycleTimer(bool enable = true);
    bool cycleTimerFlag();
    bool clearCycleTimerFlag();

    // Status
    bool voltageLow();
    uint8_t readStatus1();
    uint8_t readStatus2();

private:
    TwoWire *_wire;
    uint8_t _addr;

    bool writeRegs(uint8_t reg, const uint8_t *buf, uint8_t len);
    bool readRegs(uint8_t reg, uint8_t *buf, uint8_t len);
    uint8_t dec2bcd(uint8_t v) { return ((v / 10) << 4) | (v % 10); }
    uint8_t bcd2dec(uint8_t v) { return ((v >> 4) * 10) + (v & 0x0F); }
};