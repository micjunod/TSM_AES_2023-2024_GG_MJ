// Copyright 2023 GRAC Guilhem JUNOD Mickael

#include "pedal_device.hpp"

#include <chrono>

#include "joystick.hpp"
#include "mbed_trace.h"

namespace static_scheduling_with_event {
static constexpr std::chrono::microseconds kTaskRunTime = 200000us;

PedalDevice::PedalDevice(Timer& timer) : _timer(timer) {
    disco::Joystick::getInstance().setRightCallback(
        callback(this, &PedalDevice::onRight));
    disco::Joystick::getInstance().setLeftCallback(callback(this, &PedalDevice::onLeft));
}

// Despite what cppcheck says, this function is used in bike-system
std::chrono::milliseconds PedalDevice::getCurrentRotationTime() {
    return _pedalRotationTime;
}

void PedalDevice::onRight() {
    _pedalrtMutex.lock();
    if (_pedalRotationTime > bike_computer::kMinPedalRotationTime) {
        _pedalRotationTime -= bike_computer::kDeltaPedalRotationTime;
    }
    _pedalrtMutex.unlock();
}

void PedalDevice::onLeft() {
    _pedalrtMutex.lock();
    if (_pedalRotationTime < bike_computer::kMaxPedalRotationTime) {
        _pedalRotationTime += bike_computer::kDeltaPedalRotationTime;
    }
    _pedalrtMutex.unlock();
}
}  // namespace static_scheduling_with_event
