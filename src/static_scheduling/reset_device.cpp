// Copyright 2023 GRAC Guilhem JUNOD Mickael

// includes come here

#include "reset_device.hpp"

#if defined(TARGET_DISCO_H747I)
#define PUSH_BUTTON BUTTON1
static constexpr uint8_t kPolarityPressed = 1;
#endif

static constexpr std::chrono::microseconds kTaskRunTime = 100000us;

namespace static_scheduling {

ResetDevice::ResetDevice(Timer& timer)
    : _timer(timer), _resetButton(BUTTON1), _pressTime(0us) {
    _resetButton.rise(callback(this, &ResetDevice::onRise));
}

bool ResetDevice::checkReset() {
    std::chrono::microseconds initialTime = _timer.elapsed_time();
    std::chrono::microseconds elapsedTime = std::chrono::microseconds::zero();

    bool hasChanged = false;
    while (elapsedTime < kTaskRunTime) {
        if (!hasChanged) {
            if (_resetButton.read() == kPolarityPressed) {
                hasChanged = true;
            }
        }
        elapsedTime = _timer.elapsed_time() - initialTime;
    }
    return hasChanged;
}

std::chrono::microseconds ResetDevice::getPressTime() { return _pressTime; }

void ResetDevice::onRise() { _pressTime = _timer.elapsed_time(); }

}  // namespace static_scheduling
