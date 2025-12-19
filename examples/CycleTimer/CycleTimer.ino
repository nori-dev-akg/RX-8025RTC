/**
 * @file CycleTimer.ino
 * @brief Demonstrates the periodic timer interrupt on /INTA with initial time setup.
 */

#include <Wire.h>
#include <RX-8025RTC.h>

// Periodic timer triggers /INTA pin
const int RTC_INTA_PIN = 2;

RX8025RTC rtc(Wire);

// Flag for Interrupt Service Routine
volatile bool timerFired = false;

/**
 * @brief ISR: Just set the flag. 
 * Avoid I2C communication inside ISR to prevent deadlocks.
 */
void rtcISR() {
    timerFired = true;
}

void setup() {
    Serial.begin(9600);
    while (!Serial); // Wait for serial connection

    if (!rtc.begin()) {
        Serial.println(F("RTC not found! Check wiring."));
        while (1);
    }

    // ---- Initial Time Setup ----
    // Set to 12:00:00 Friday for testing purposes
    rtc.setTime(12, 0, 0);
    rtc.setDate(25, 12, 19, W_FRI);

    // ---- Interrupt Pin Configuration ----
    pinMode(RTC_INTA_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(RTC_INTA_PIN), rtcISR, FALLING);

    // ---- Periodic Timer Setup ----
    // CYCLE_1HZ_P: Generates a pulse every 1 second
    rtc.setCycleTimer(RX8025RTC::CYCLE_1HZ_P);
    rtc.enableCycleTimer(true);
    rtc.clearCycleTimerFlag();

    Serial.println(F("Initial time set to 12:00:00."));
    Serial.println(F("Periodic Timer (1Hz) started on /INTA."));
}

void loop() {
    // Check if interrupt flag was set by ISR
    if (timerFired) {
        // Verify RTC status register to confirm it was a cycle timer event
        if (rtc.cycleTimerFlag()) {
            Serial.print(F("Timer Tick! Current Time: "));
            
            uint8_t h, m, s;
            if (rtc.getTime(h, m, s)) {
                if (h < 10) Serial.print('0'); Serial.print(h); Serial.print(':');
                if (m < 10) Serial.print('0'); Serial.print(m); Serial.print(':');
                if (s < 10) Serial.print('0'); Serial.print(s);
            }
            Serial.println();

            // Clear the flag to allow the next interrupt pulse
            rtc.clearCycleTimerFlag();
        }
        // Reset the local flag
        timerFired = false;
    }
}