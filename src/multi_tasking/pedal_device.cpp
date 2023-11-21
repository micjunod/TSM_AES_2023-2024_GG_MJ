// Copyright 2023 GRAC Guilhem JUNOD Mickael

#include "pedal_device.hpp"

#include <chrono>

#include "joystick.hpp"
#include "mbed_trace.h"

namespace multi_tasking {
static constexpr std::chrono::microseconds kTaskRunTime = 200000us;

PedalDevice::PedalDevice(Timer& timer) : _timer(timer) {
    disco::Joystick::getInstance().setRightCallback(
        callback(this, &PedalDevice::onRight));
    disco::Joystick::getInstance().setLeftCallback(callback(this, &PedalDevice::onLeft));
}

std::chrono::milliseconds PedalDevice::getCurrentRotationTime() {
    return _pedalRotationTime;
}

void PedalDevice::onRight() {
    if (_pedalRotationTime > bike_computer::kMinPedalRotationTime) {
        _pedalRotationTime -= bike_computer::kDeltaPedalRotationTime;
    }
}

void PedalDevice::onLeft() {
    if (_pedalRotationTime < bike_computer::kMaxPedalRotationTime) {
        _pedalRotationTime += bike_computer::kDeltaPedalRotationTime;
    }
}
}  // namespace multi_tasking
