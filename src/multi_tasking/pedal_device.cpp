// Copyright 2023 GRAC Guilhem JUNOD Mickael

#include "pedal_device.hpp"

#include <chrono>

#include "joystick.hpp"
#include "mbed_trace.h"

namespace multi_tasking {
static constexpr std::chrono::microseconds kTaskRunTime = 200000us;

PedalDevice::PedalDevice(mbed::Callback<void()> cbLeft, mbed::Callback<void()> cbRight) {
    disco::Joystick::getInstance().setRightCallback(cbRight);
    disco::Joystick::getInstance().setLeftCallback(cbLeft);
}

std::chrono::milliseconds PedalDevice::getCurrentRotationTime() {
    return _pedalRotationTime;
}

void PedalDevice::decrementPedal() {
    if (_pedalRotationTime > bike_computer::kMinPedalRotationTime) {
        _pedalRotationTime -= bike_computer::kDeltaPedalRotationTime;
    }
}

void PedalDevice::incrementPedal() {
    if (_pedalRotationTime < bike_computer::kMaxPedalRotationTime) {
        _pedalRotationTime += bike_computer::kDeltaPedalRotationTime;
    }
}
}  // namespace multi_tasking
