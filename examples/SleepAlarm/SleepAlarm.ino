/**
 * @file SleepAlarm.ino
 * @brief Clean Sleep wakeup for Arduino Uno.
 * ISRs only set flags. All logic and sleep management stay in loop().
 */

#include <Wire.h>
#include <RX-8025RTC.h>
#include <avr/sleep.h>

const int RTC_INTA_PIN = 2; 
const int RTC_INTB_PIN = 3; 

RX8025RTC rtc(Wire);

// Flags marked 'volatile' for ISR
volatile bool wokeByA = false;
volatile bool wokeByB = false;

// ISR: Minimalist approach - JUST set the flag.
void rtcISRA() { wokeByA = true; }
void rtcISRB() { wokeByB = true; }

void setup() {
    Serial.begin(9600);
    while (!Serial);

    if (!rtc.begin()) {
        Serial.println(F("RTC not found!"));
        while (1);
    }

    pinMode(RTC_INTA_PIN, INPUT_PULLUP);
    pinMode(RTC_INTB_PIN, INPUT_PULLUP);

    // Initial RTC Setup: 2025/12/19 12:00:00 Friday
    rtc.setTime(12, 0, 0);
    rtc.setDate(25, 12, 19, W_FRI);

    // Alarm D (+1 min) -> /INTA
    rtc.setAlarmD(12, 1);
    rtc.enableAlarmD(true);
    rtc.clearAlarmDFlag();

    // Alarm W (+2 min) -> /INTB
    rtc.setAlarmW(12, 2, BIT_FRI);
    rtc.enableAlarmW(true);
    rtc.clearAlarmWFlag();

    Serial.println(F("Alarms set. Arduino is going to sleep."));
    Serial.flush();
}

void loop() {
    // 1. Setup Sleep Mode
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    
    // 2. Attach interrupts (LOW level for Power-down wakeup)
    attachInterrupt(digitalPinToInterrupt(RTC_INTA_PIN), rtcISRA, LOW);
    attachInterrupt(digitalPinToInterrupt(RTC_INTB_PIN), rtcISRB, LOW);

    // 3. Enter Sleep
    sleep_enable();
    sleep_cpu();

    // --- Arduino Wakes Up Here ---

    // 4. Clean up immediately
    sleep_disable();
    detachInterrupt(digitalPinToInterrupt(RTC_INTA_PIN));
    detachInterrupt(digitalPinToInterrupt(RTC_INTB_PIN));

    // 5. Check flags and process
    if (wokeByA) {
        if (rtc.alarmDFlag()) {
            Serial.println(F("Wakeup Event: Alarm D matched."));
            rtc.clearAlarmDFlag();
        }
        wokeByA = false;
    }

    if (wokeByB) {
        if (rtc.alarmWFlag()) {
            Serial.println(F("Wakeup Event: Alarm W matched."));
            rtc.clearAlarmWFlag();
        }
        wokeByB = false;
    }

    Serial.println(F("Processing complete. Re-entering sleep in 2s..."));
    Serial.flush();
    delay(2000); 
}