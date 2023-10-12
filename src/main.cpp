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

#include <cstdio>

#include "mbed.h"
#include "mbed_trace.h"
#if defined(MBED_CONF_MBED_TRACE_ENABLE)
#define TRACE_GROUP "MAIN"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

// Blinking rate in milliseconds
#define BLINKING_RATE 500ms

int division(int x) {
    if (x == 0) return 0;
    return 10 / x;
}

#if !MBED_TEST_MODE
int main() {
#if defined(MBED_CONF_MBED_TRACE_ENABLE)
    mbed_trace_init();
#endif

    int val    = 3;
    float fval = val * 4.0;
    /* val = (int)fval; */
    val = static_cast<int>(fval);

    int div  = division(2);
    int div2 = division(0);

    // Initialise the digital pin LED1 as an output
#ifdef LED1
    DigitalOut led(LED1);
#else
    bool led = false;
#endif

    while (true) {
        led = !led;
        ThisThread::sleep_for(BLINKING_RATE);
        tr_info("App running");
    }
}
#endif
