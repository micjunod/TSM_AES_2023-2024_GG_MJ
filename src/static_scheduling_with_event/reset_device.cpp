// Copyright 2023 GRAC Guilhem JUNOD Mickael

// includes come here

#include "reset_device.hpp"

#if defined(TARGET_DISCO_H747I)
#define PUSH_BUTTON BUTTON1
static constexpr uint8_t kPolarityPressed = 1;
#endif

static constexpr std::chrono::microseconds kTaskRunTime = 100000us;

namespace static_scheduling_with_event {

ResetDevice::ResetDevice(mbed::Callback<void()> cb) : _resetButton(BUTTON1) {
    _resetButton.rise(cb);
}

}  // namespace static_scheduling_with_event
