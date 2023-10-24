// Copyright 2023 GRAC Guilhem JUNOD Mickael

// includes come here

#include "reset_device.hpp"

#if defined(TARGET_DISCO_H747I)
#define PUSH_BUTTON BUTTON1
static constexpr uint8_t kPolarityPressed = 1;
#endif

namespace static_scheduling {

ResetDevice::ResetDevice(Timer& timer)
    : _timer(timer), _resetButton(BUTTON1), _pressTime(0us) {
    _resetButton.rise(callback(this, &ResetDevice::onRise));
}

bool ResetDevice::checkReset() { return (_resetButton.read() == kPolarityPressed); }

std::chrono::microseconds ResetDevice::getPressTime() { return _pressTime; }

void ResetDevice::onRise() { _pressTime = _timer.elapsed_time(); }

}  // namespace static_scheduling
