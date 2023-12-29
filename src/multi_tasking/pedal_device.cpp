// Copyright 2023 GRAC Guilhem JUNOD Mickael

#include "pedal_device.hpp"

#include <chrono>

#include "joystick.hpp"
#include "mbed_trace.h"
#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "PedalDevice"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

namespace multi_tasking {
static constexpr std::chrono::microseconds kTaskRunTime = 200000us;

PedalDevice::PedalDevice(mbed::Callback<void()> cbLeft, mbed::Callback<void()> cbRight) {
    disco::Joystick::getInstance().setRightCallback(cbRight);
    disco::Joystick::getInstance().setLeftCallback(cbLeft);
    tr_info("%" PRIu64, _pedalRotationTime);
}

std::chrono::milliseconds PedalDevice::getCurrentRotationTime() {
    return std::chrono::milliseconds(core_util_atomic_load_u64(&_pedalRotationTime));
}

void PedalDevice::decrementPedal() {
    if (core_util_atomic_load_u64(&_pedalRotationTime) >
        bike_computer::kMinPedalRotationTime.count()) {
        core_util_atomic_decr_u64(&_pedalRotationTime,
                                  bike_computer::kDeltaPedalRotationTime.count());
    }
}

void PedalDevice::incrementPedal() {
    if (core_util_atomic_load_u64(&_pedalRotationTime) <
        bike_computer::kMaxPedalRotationTime.count()) {
        core_util_atomic_incr_u64(&_pedalRotationTime,
                                  bike_computer::kDeltaPedalRotationTime.count());
    }
}
}  // namespace multi_tasking
