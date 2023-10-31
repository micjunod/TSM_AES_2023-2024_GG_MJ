// Copyright 2023 GRAC Guilhem JUNOD Mickael

#include "pedal_device.hpp"

#include <chrono>

#include "joystick.hpp"
#include "mbed_trace.h"

namespace static_scheduling {
static constexpr std::chrono::microseconds kTaskRunTime = 200000us;

PedalDevice::PedalDevice(Timer& timer) : _timer(timer) {}

std::chrono::milliseconds PedalDevice::getCurrentRotationTime() {
    std::chrono::microseconds initialTime = _timer.elapsed_time();
    std::chrono::microseconds elapsedTime = std::chrono::microseconds::zero();
    // we bound the change to one increment/decrement per call
    bool hasChanged = false;

    while (elapsedTime < kTaskRunTime) {
        if (!hasChanged) {
            disco::Joystick::State joystickState =
                disco::Joystick::getInstance().getState();
            switch (joystickState) {
                case disco::Joystick::State::RightPressed:
                    increaseRotationSpeed();
                    hasChanged = true;
                    break;
                case disco::Joystick::State::LeftPressed:
                    decreaseRotationSpeed();
                    hasChanged = true;
                    break;
                default:
                    break;
            }
        }
        elapsedTime = _timer.elapsed_time() - initialTime;
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
}  // namespace static_scheduling
