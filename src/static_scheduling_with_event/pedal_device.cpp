// Copyright 2023 GRAC Guilhem JUNOD Mickael

#include "pedal_device.hpp"

#include <chrono>

#include "mbed_trace.h"

namespace static_scheduling_with_event {
static constexpr std::chrono::microseconds kTaskRunTime = 200000us;

PedalDevice::PedalDevice(mbed::Callback<void()> cb) {
    _joystick.setLeftCallback(cb);
    _joystick.setRightCallback(cb);
}

std::chrono::milliseconds PedalDevice::getCurrentRotationTime() {
    disco::Joystick::State joystickState = disco::Joystick::getInstance().getState();

    switch (joystickState) {
        case disco::Joystick::State::RightPressed:
            increaseRotationSpeed();
            break;
        case disco::Joystick::State::LeftPressed:
            decreaseRotationSpeed();
            break;
        default:
            break;
    }
    return _pedalRotationTime;
}

void PedalDevice::increaseRotationSpeed() {
    if (_pedalRotationTime > bike_computer::kMinPedalRotationTime) {
        _pedalRotationTime -= bike_computer::kDeltaPedalRotationTime;
    }
}

void PedalDevice::decreaseRotationSpeed() {
    if (_pedalRotationTime < bike_computer::kMaxPedalRotationTime) {
        _pedalRotationTime += bike_computer::kDeltaPedalRotationTime;
    }
}
}  // namespace static_scheduling_with_event
