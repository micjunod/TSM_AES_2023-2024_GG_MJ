/**
 ******************************************************************************
 * @file        : main.cpp
 * @brief       : Led blinky
 * @author      : Mickael Junod
 * @date        : 10.10.2023
 ******************************************************************************
 * @copyright   : Copyright (c) 1503
 *                HES-SO lausanne
 * @attention   : SPDX-License-Identifier: MIT OR Apache-2.0
 ******************************************************************************
 * @details
 * blinky led
 ******************************************************************************
 */
#include "mbed.h"
#include "mbed_trace.h"
#if defined(MBED_CONF_MBED_TRACE_ENABLE)
#define TRACE_GROUP "MAIN"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

#include "multi_tasking/bike_system.hpp"
#include "static_scheduling/bike_system.hpp"
#include "static_scheduling_with_event/bike_system.hpp"

// Blinking rate in milliseconds
#define BLINKING_RATE 500ms

#if !MBED_TEST_MODE
int main() {
#if defined(MBED_CONF_MBED_TRACE_ENABLE)
    mbed_trace_init();
#endif

#ifdef LED1
    DigitalOut led(LED1);
#else
    bool led = false;
#endif

    multi_tasking::BikeSystem bikeSystem;
    bikeSystem.start();

    while (true) {
        led = !led;
        ThisThread::sleep_for(BLINKING_RATE);
        tr_info("App running");
    }
}
#endif
