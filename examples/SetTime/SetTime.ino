/**
 * @file SetTime.ino
 * @brief Simple example to set and read time/date using Enum names.
 */

#include <Wire.h>
#include <RX-8025RTC.h>

// Create RTC instance
RX8025RTC rtc(Wire);

void setup() {
    Serial.begin(115200);
    while (!Serial); // Wait for serial monitor to open (for boards with native USB)

    Serial.println(F("RX-8025 RTC SetTime Example"));

    // Initialize RTC
    if (!rtc.begin()) {
        Serial.println(F("Couldn't find RX-8025 RTC. Check your wiring!"));
        while (1);
    }

    // 1. Check for Power Failure / Low Voltage
    if (rtc.voltageLow()) {
        Serial.println(F("Warning: RTC voltage was low. Backup battery may be empty."));
    }

    // 2. Set Time: 14:30:05 (24-hour format)
    if (rtc.setTime(14, 30, 5)) {
        Serial.println(F("Time set successfully."));
    }

    // 3. Set Date: 2025-12-19 (Friday)
    // Use the Weekday enum (W_SUN, W_MON, ..., W_SAT) for clarity
    if (rtc.setDate(25, 12, 19, W_FRI)) {
        Serial.println(F("Date set successfully."));
    }

    Serial.println(F("------------------------------------"));
}

void loop() {
    uint8_t hour, min, sec;
    uint8_t year, month, day;
    Weekday wday; // Enum type

    // Read current time and date
    if (rtc.getTime(hour, min, sec) && rtc.getDate(year, month, day, wday)) {
        
        // Format Date: 20YY/MM/DD
        Serial.print(F("Date: 20"));
        if (year < 10) Serial.print('0');
        Serial.print(year);
        Serial.print('/');
        if (month < 10) Serial.print('0');
        Serial.print(month);
        Serial.print('/');
        if (day < 10) Serial.print('0');
        Serial.print(day);

        // Print Weekday string
        const char* days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
        Serial.print(F(" ("));
        if (wday >= 0 && wday <= 6) {
            Serial.print(days[wday]);
        }
        Serial.print(F(") "));

        // Format Time: HH:MM:SS
        if (hour < 10) Serial.print('0');
        Serial.print(hour);
        Serial.print(':');
        if (min < 10) Serial.print('0');
        Serial.print(min);
        Serial.print(':');
        if (sec < 10) Serial.print('0');
        Serial.print(sec);

        Serial.println();
    } else {
        Serial.println(F("Error: Could not read from RTC."));
    }

    delay(1000); 
}