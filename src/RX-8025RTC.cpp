/**
 * @file RX-8025RTC.cpp
 * @brief Implementation of the RX-8025SA/NB RTC driver.
 * @author nori-dev-akg
 * @license MIT
 */

#include "RX-8025RTC.h"

/**
 * @brief Constructor
 */
RX8025RTC::RX8025RTC(TwoWire &wire, uint8_t i2cAddr) 
    : _wire(&wire), _addr(i2cAddr) {}

/**
 * @brief Initializes I2C and performs a basic communication check.
 */
bool RX8025RTC::begin() {
    _wire->begin();
    
    uint8_t ctrl1;
    if (readRegs(REG_CTRL1, &ctrl1, 1)) {
        ctrl1 &= ~0x20; // 12/24H bit = 0 (24H)
        writeRegs(REG_CTRL1, &ctrl1, 1);
    }

    uint8_t h, m, s;
    return getTime(h, m, s);
}
// ---- Time & Date ----

/**
 * @brief Sets current time (24h format).
 */
bool RX8025RTC::setTime(uint8_t hour, uint8_t min, uint8_t sec) {
    uint8_t buf[3] = { dec2bcd(sec), dec2bcd(min), dec2bcd(hour) };
    return writeRegs(REG_SEC, buf, 3);
}

/**
 * @brief Reads current time.
 */
bool RX8025RTC::getTime(uint8_t &hour, uint8_t &min, uint8_t &sec) {
    uint8_t buf[3];
    if (!readRegs(REG_SEC, buf, 3)) return false;
    sec  = bcd2dec(buf[0] & 0x7F);
    min  = bcd2dec(buf[1] & 0x7F);
    hour = bcd2dec(buf[2] & 0x3F);
    return true;
}

/**
 * @brief Sets calendar date.
 */
bool RX8025RTC::setDate(uint8_t year, uint8_t month, uint8_t day) {
    uint8_t wday = getWeekday(year, month, day);
    uint8_t buf[4] = { (uint8_t)(wday & 0x07), dec2bcd(day), dec2bcd(month), dec2bcd(year) };
    return writeRegs(REG_WEEK, buf, 4);
}

/**
 * @brief Reads calendar date.
 */
bool RX8025RTC::getDate(uint8_t &year, uint8_t &month, uint8_t &day, uint8_t &wday) {
    uint8_t buf[4];
    if (!readRegs(REG_WEEK, buf, 4)) return false;
    wday  = buf[0] & 0x07;
    day   = bcd2dec(buf[1] & 0x3F);
    month = bcd2dec(buf[2] & 0x1F);
    year  = bcd2dec(buf[3]);
    return true;
}

// ---- Alarm D (/INTA) ----

/**
 * @brief Configures Alarm D (Daily Alarm).
 */
bool RX8025RTC::setAlarmD(uint8_t hour, uint8_t min) {
    uint8_t buf[2] = { dec2bcd(min), dec2bcd(hour) }; // AE(bit7)=0 ensures alarm is active
    return writeRegs(REG_ALM_D_MIN, buf, 2);
}

/**
 * @brief Enables/Disables Alarm D (DALE bit).
 */
bool RX8025RTC::enableAlarmD(bool enable) {
    uint8_t ctrl1;
    if (!readRegs(REG_CTRL1, &ctrl1, 1)) return false;
    enable ? ctrl1 |= 0x40 : ctrl1 &= ~0x40; // bit6: DALE
    return writeRegs(REG_CTRL1, &ctrl1, 1);
}

/**
 * @brief Checks if Alarm D has triggered (DAFG bit).
 */
bool RX8025RTC::alarmDFlag() {
    return (readStatus2() & 0x01); // bit0: DAFG
}

/**
 * @brief Clears Alarm D flag.
 */
bool RX8025RTC::clearAlarmDFlag() {
    uint8_t s2 = readStatus2();
    s2 &= ~0x01;
    return writeRegs(REG_CTRL2, &s2, 1);
}

// ---- Alarm W (/INTB) ----

/**
 * @brief Configures Alarm W (Weekly Alarm).
 * @param weekMask Bitmask from WeekdayMask (Default: ALL).
 */
bool RX8025RTC::setAlarmW(uint8_t hour, uint8_t min, WeekdayMask weekMask) {
    uint8_t buf[3] = { dec2bcd(min), dec2bcd(hour), (uint8_t)weekMask };
    return writeRegs(REG_ALM_W_MIN, buf, 3);
}

/**
 * @brief Enables/Disables Alarm W (WALE bit).
 */
bool RX8025RTC::enableAlarmW(bool enable) {
    uint8_t ctrl1;
    if (!readRegs(REG_CTRL1, &ctrl1, 1)) return false;
    enable ? ctrl1 |= 0x80 : ctrl1 &= ~0x80; // bit7: WALE
    return writeRegs(REG_CTRL1, &ctrl1, 1);
}

/**
 * @brief Checks if Alarm W has triggered (WAFG bit).
 */
bool RX8025RTC::alarmWFlag() {
    return (readStatus2() & 0x02); // bit1: WAFG
}

/**
 * @brief Clears Alarm W flag.
 */
bool RX8025RTC::clearAlarmWFlag() {
    uint8_t s2 = readStatus2();
    s2 &= ~0x02;
    return writeRegs(REG_CTRL2, &s2, 1);
}

// ---- Periodic Interrupt (/INTA) ----

/**
 * @brief Sets the cycle for periodic interrupt (CT2-0).
 */
bool RX8025RTC::setCycleTimer(CyclePeriod period) {
    uint8_t ctrl1;
    if (!readRegs(REG_CTRL1, &ctrl1, 1)) return false;
    ctrl1 &= 0xF8; // Clear CT bits
    ctrl1 |= (uint8_t)period;
    return writeRegs(REG_CTRL1, &ctrl1, 1);
}

/**
 * @brief Enables/Disables periodic interrupt (CTE bit).
 */
bool RX8025RTC::enableCycleTimer(bool enable) {
    uint8_t ctrl2;
    if (!readRegs(REG_CTRL2, &ctrl2, 1)) return false;
    enable ? ctrl2 |= 0x40 : ctrl2 &= ~0x40; // bit6: CTE
    return writeRegs(REG_CTRL2, &ctrl2, 1);
}

/**
 * @brief Checks if Periodic Timer has triggered (CTFG bit).
 */
bool RX8025RTC::cycleTimerFlag() {
    return (readStatus2() & 0x04); // bit2: CTFG
}

/**
 * @brief Clears Periodic Timer flag.
 */
bool RX8025RTC::clearCycleTimerFlag() {
    uint8_t s2 = readStatus2();
    s2 &= ~0x04;
    return writeRegs(REG_CTRL2, &s2, 1);
}

// ---- Status ----

/**
 * @brief Returns true if Voltage Low Detection flag (VDET/VLF) is set.
 */
bool RX8025RTC::voltageLow() {
    return (readStatus2() & 0x80); // bit7: VDET
}

/**
 * @brief Reads Control 1 (Standard status).
 */
uint8_t RX8025RTC::readStatus1() {
    uint8_t s;
    readRegs(REG_CTRL1, &s, 1);
    return s;
}

/**
 * @brief Reads Control 2 (Flags status).
 */
uint8_t RX8025RTC::readStatus2() {
    uint8_t s;
    readRegs(REG_CTRL2, &s, 1);
    return s;
}

// ---- I2C Communication ----

/**
 * @brief Writes data to consecutive registers.
 * Hardware requires Address in upper 4 bits of the first byte.
 */
bool RX8025RTC::writeRegs(uint8_t reg, const uint8_t *buf, uint8_t len) {
    _wire->beginTransmission(_addr);
    _wire->write(((reg & 0x0F) << 4) | ((reg & 0xF0) >> 4)); 
    _wire->write(buf, len);
    return _wire->endTransmission() == 0;
}

/**
 * @brief Reads data from consecutive registers.
 */
bool RX8025RTC::readRegs(uint8_t reg, uint8_t *buf, uint8_t len) {
    _wire->beginTransmission(_addr);
    _wire->write(((reg & 0x0F) << 4) | ((reg & 0xF0) >> 4));
    if (_wire->endTransmission(false) != 0) return false;
    _wire->requestFrom(_addr, len);
    for (uint8_t i = 0; i < len; i++) {
        if (_wire->available()) buf[i] = _wire->read();
    }
    return true;
}

/**
 * @brief Calculates the day of the week for a given date.
 * 
 * @param y year (0-99)
 * @param m month (1-12)
 * @param d day (1-31)
 * @return int 
 */
int RX8025RTC::getWeekday(int y, int m, int d)
{
    y += 2000;
    if (m < 3)
    {
        y--;
        m += 12;
    }
    return (y + y / 4 - y / 100 + y / 400 + (13 * m + 8) / 5 + d) % 7;
}
