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

#include "FlashIAPBlockDevice.h"
#include "multi_tasking/bike_system.hpp"
#include "static_scheduling/bike_system.hpp"
#include "static_scheduling_with_event/bike_system.hpp"
#include "update-client/block_device_application.hpp"
#include "update-client/uc_error_code.hpp"
#include "update-client/usb_serial_uc.hpp"

using namespace update_client;

// Blinking rate in milliseconds
#define BLINKING_RATE 500ms

const char szMsg[]            = "This is a test message";
static constexpr uint8_t size = 10;
uint32_t randomArray[size]    = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
uint32_t randomNumber         = 0;

#if !MBED_TEST_MODE
int main() {
#if defined(MBED_CONF_MBED_TRACE_ENABLE)
    mbed_trace_init();
#endif

    tr_info(szMsg);
    for (uint8_t i = 0; i < size; i++) {
        randomArray[i] = rand();  // NOLINT
        tr_info("This is a random number %u", randomArray[i]);
    }
    randomNumber = rand();  // NOLINT
    tr_info("This is a random number %u", randomNumber);

    /*FlashIAPBlockDevice flashIAPBlockDevice(MBED_ROM_START, MBED_ROM_SIZE);
    USBSerialUC usbSerialUpdateClient(flashIAPBlockDevice);
    UCErrorCode rc = usbSerialUpdateClient.start();
    if (rc != UCErrorCode::UC_ERR_NONE) {
        tr_error("Cannot initialize update client: %d", rc);
    } else {
        tr_info("Update client started");
    }*/

    tr_info("VERSION : 1");

    multi_tasking::BikeSystem bikeSystem;
    bikeSystem.start();
}
#endif
