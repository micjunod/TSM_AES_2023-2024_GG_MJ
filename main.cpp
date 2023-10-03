/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"

#include "mbed_trace.h"
#if defined(MBED_CONF_MBED_TRACE_ENABLE)
#define TRACE_GROUP  "MAIN"
#endif // MBED_CONF_MBED_TRACE_ENABLE

// Blinking rate in milliseconds
#define BLINKING_RATE     500ms

#if !MBED_TEST_MODE
int main()
{

    #if defined(MBED_CONF_MBED_TRACE_ENABLE)
        mbed_trace_init();
    #endif

        // Initialise the digital pin LED1 as an output
    #ifdef LED1
        DigitalOut led(LED1);
    #else
        bool led;
    #endif
    
    while (true) 
    {
        led = !led;
        ThisThread::sleep_for(BLINKING_RATE);
        tr_info("App running");
    }
}
#endif
