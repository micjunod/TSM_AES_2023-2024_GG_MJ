// Copyright 2022 Haute école d'ingénierie et d'architecture de Fribourg
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/****************************************************************************
 * @file bike_system.cpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Bike System implementation (static scheduling)
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#include "static_scheduling_with_event/bike_system.hpp"

#include <chrono>

#include "mbed_trace.h"
#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "BikeSystem"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

namespace static_scheduling_with_event {

static constexpr std::chrono::milliseconds kGearTaskPeriod                   = 800ms;
static constexpr std::chrono::milliseconds kGearTaskDelay                    = 0ms;
static constexpr std::chrono::milliseconds kGearTaskComputationTime          = 100ms;
static constexpr std::chrono::milliseconds kSpeedDistanceTaskPeriod          = 400ms;
static constexpr std::chrono::milliseconds kSpeedDistanceTaskDelay           = 0ms;
static constexpr std::chrono::milliseconds kSpeedDistanceTaskComputationTime = 200ms;
static constexpr std::chrono::milliseconds kDisplayTask1Period               = 1600ms;
static constexpr std::chrono::milliseconds kDisplayTask1Delay                = 300ms;
static constexpr std::chrono::milliseconds kDisplayTask1ComputationTime      = 200ms;
static constexpr std::chrono::milliseconds kResetTaskPeriod                  = 800ms;
static constexpr std::chrono::milliseconds kResetTaskDelay                   = 700ms;
static constexpr std::chrono::milliseconds kResetTaskComputationTime         = 100ms;
static constexpr std::chrono::milliseconds kTemperatureTaskPeriod            = 1600ms;
static constexpr std::chrono::milliseconds kTemperatureTaskDelay             = 1100ms;
static constexpr std::chrono::milliseconds kTemperatureTaskComputationTime   = 100ms;
static constexpr std::chrono::milliseconds kDisplayTask2Period               = 1600ms;
static constexpr std::chrono::milliseconds kDisplayTask2Delay                = 1200ms;
static constexpr std::chrono::milliseconds kDisplayTask2ComputationTime      = 100ms;
static constexpr std::chrono::milliseconds kMajorCycleDuration               = 1600ms;

BikeSystem::BikeSystem()
    : _resetDevice(callback(this, &BikeSystem::onReset)),
      _displayDevice(),
      _speedometer(_timer),
      _sensorDevice(),
      _gearDevice(callback(this, &BikeSystem::changeGear)),
      _pedalDevice(callback(this, &BikeSystem::changePedal)),
      _cpuLogger(_timer) {}

void BikeSystem::start() {
    tr_info("Starting Super-Loop without event handling");

    init();

    while (true) {
        auto startTime = _timer.elapsed_time();

        speedDistanceTask();
        resetTask();
        gearTask();

        speedDistanceTask();
        temperatureTask();
        displayTask2();

        speedDistanceTask();
        resetTask();
        gearTask();

        speedDistanceTask();
        displayTask1();

        _cpuLogger.printStats();

        // register the time at the end of the cyclic schedule period and print the
        // elapsed time for the period
        std::chrono::microseconds endTime = _timer.elapsed_time();
        const auto cycle =
            std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        tr_debug("Repeating cycle time is %" PRIu64 " milliseconds", cycle.count());

        // implement loop exit when applicable
    }
}

void BikeSystem::startWithEventQueue() {
    tr_info("Starting Super-Loop WITH event handling");

    init();

    EventQueue eventQueue;

    Event<void()> gearEvent(&eventQueue, callback(this, &BikeSystem::gearTask));
    gearEvent.delay(kGearTaskDelay);
    gearEvent.period(kGearTaskPeriod);
    gearEvent.post();

    Event<void()> speedDistanceEvent(&eventQueue,
                                     callback(this, &BikeSystem::speedDistanceTask));
    speedDistanceEvent.delay(kSpeedDistanceTaskDelay);
    speedDistanceEvent.period(kSpeedDistanceTaskPeriod);
    speedDistanceEvent.post();

    Event<void()> temperatureEvent(&eventQueue,
                                   callback(this, &BikeSystem::temperatureTask));
    temperatureEvent.delay(kTemperatureTaskDelay);
    temperatureEvent.period(kTemperatureTaskPeriod);
    temperatureEvent.post();

    Event<void()> resetEvent(&eventQueue, callback(this, &BikeSystem::resetTask));
    resetEvent.delay(kResetTaskDelay);
    resetEvent.period(kResetTaskPeriod);
    resetEvent.post();

    Event<void()> displayEvent1(&eventQueue, callback(this, &BikeSystem::displayTask1));
    displayEvent1.delay(kDisplayTask1Delay);
    displayEvent1.period(kDisplayTask1Period);
    displayEvent1.post();

    Event<void()> displayEvent2(&eventQueue, callback(this, &BikeSystem::displayTask2));
    displayEvent2.delay(kDisplayTask2Delay);
    displayEvent2.period(kDisplayTask2Period);
    displayEvent2.post();

#if !MBED_TEST_MODE
    Event<void()> cpuStatsEvent(&eventQueue,
                                callback(&_cpuLogger, &advembsof::CPULogger::printStats));
    cpuStatsEvent.delay(kMajorCycleDuration);
    cpuStatsEvent.period(kMajorCycleDuration);
    cpuStatsEvent.post();
#endif
    // register the time at the end of the cyclic schedule period and print the
    // elapsed time for the period
    eventQueue.dispatch_forever();
}

void BikeSystem::stop() { core_util_atomic_store_bool(&_stopFlag, true); }

#if defined(MBED_TEST_MODE)
const advembsof::TaskLogger& BikeSystem::getTaskLogger() { return _taskLogger; }
#endif  // defined(MBED_TEST_MODE)

void BikeSystem::init() {
    // start the timer
    _timer.start();

    // initialize the lcd display
    disco::ReturnCode rc = _displayDevice.init();
    if (rc != disco::ReturnCode::Ok) {
        tr_error("Failed to initialized the lcd display: %d", static_cast<int>(rc));
    }

    // initialize the sensor device
    bool present = _sensorDevice.init();
    if (!present) {
        tr_error("Sensor not present or initialization failed");
    }

    // enable/disable task logging
    _taskLogger.enable(true);
}

void BikeSystem::changeGear() {
    core_util_atomic_store_bool(&_isJoystickGear, true);
    _joystickGearTime = _timer.elapsed_time();
}

void BikeSystem::gearTask() {
    auto taskStartTime = _timer.elapsed_time();

    if (core_util_atomic_load_bool(&_isJoystickGear)) {
        std::chrono::microseconds responseTime =
            _timer.elapsed_time() - _joystickGearTime;
        tr_info("Reset task: response time is %" PRIu64 " usecs", responseTime.count());
        _currentGear     = _gearDevice.getCurrentGear();
        _currentGearSize = _gearDevice.getCurrentGearSize();
        core_util_atomic_store_bool(&_isJoystickGear, false);
    }
    _taskLogger.logPeriodAndExecutionTime(
        _timer, advembsof::TaskLogger::kGearTaskIndex, taskStartTime);
}

void BikeSystem::changePedal() {
    core_util_atomic_store_bool(&_isJoystickPedal, true);
    _joystickPedalTime = _timer.elapsed_time();
}

void BikeSystem::speedDistanceTask() {
    // speed and distance task
    auto taskStartTime = _timer.elapsed_time();

    if (core_util_atomic_load_bool(&_isJoystickPedal)) {
        std::chrono::microseconds responseTime =
            _timer.elapsed_time() - _joystickPedalTime;
        tr_info("Pedal task: response time is %" PRIu64 " usecs", responseTime.count());
        core_util_atomic_store_bool(&_isJoystickPedal, false);
        const auto pedalRotationTime = _pedalDevice.getCurrentRotationTime();
        _speedometer.setCurrentRotationTime(pedalRotationTime);
    }
    _speedometer.setGearSize(_currentGearSize);
    // no need to protect access to data members (single threaded)
    _currentSpeed     = _speedometer.getCurrentSpeed();
    _traveledDistance = _speedometer.getDistance();

    _taskLogger.logPeriodAndExecutionTime(
        _timer, advembsof::TaskLogger::kSpeedTaskIndex, taskStartTime);
}

void BikeSystem::temperatureTask() {
    auto taskStartTime = _timer.elapsed_time();

    // no need to protect access to data members (single threaded)
    _currentTemperature = _sensorDevice.readTemperature();

    auto elapsedTimeTask = std::chrono::duration_cast<std::chrono::milliseconds>(
        _timer.elapsed_time() - taskStartTime);

    // simulate task computation by waiting for the required task computation time
    ThisThread::sleep_for(kTemperatureTaskComputationTime - elapsedTimeTask);

    _taskLogger.logPeriodAndExecutionTime(
        _timer, advembsof::TaskLogger::kTemperatureTaskIndex, taskStartTime);
}

void BikeSystem::onReset() {
    core_util_atomic_store_bool(&_isReset, true);
    _resetTime = _timer.elapsed_time();
}

void BikeSystem::resetTask() {
    auto taskStartTime = _timer.elapsed_time();

    if (core_util_atomic_load_bool(&_isReset)) {
        std::chrono::microseconds responseTime = _timer.elapsed_time() - _resetTime;
        tr_info("Reset task: response time is %" PRIu64 " usecs", responseTime.count());
        _speedometer.reset();
        core_util_atomic_store_bool(&_isReset, false);
    }

    _taskLogger.logPeriodAndExecutionTime(
        _timer, advembsof::TaskLogger::kResetTaskIndex, taskStartTime);
}

void BikeSystem::displayTask1() {
    auto taskStartTime = _timer.elapsed_time();

    _displayDevice.displayGear(_currentGear);
    _displayDevice.displaySpeed(_currentSpeed);
    _displayDevice.displayDistance(_traveledDistance);

    auto elapsedTimeTask = std::chrono::duration_cast<std::chrono::milliseconds>(
        _timer.elapsed_time() - taskStartTime);

    // simulate task computation by waiting for the required task computation time

    ThisThread::sleep_for(kDisplayTask1ComputationTime - elapsedTimeTask);

    _taskLogger.logPeriodAndExecutionTime(
        _timer, advembsof::TaskLogger::kDisplayTask1Index, taskStartTime);
}

void BikeSystem::displayTask2() {
    auto taskStartTime = _timer.elapsed_time();

    _displayDevice.displayTemperature(_currentTemperature);

    auto elapsedTimeTask = std::chrono::duration_cast<std::chrono::milliseconds>(
        _timer.elapsed_time() - taskStartTime);

    // simulate task computation by waiting for the required task computation time

    ThisThread::sleep_for(kDisplayTask2ComputationTime - elapsedTimeTask);

    _taskLogger.logPeriodAndExecutionTime(
        _timer, advembsof::TaskLogger::kDisplayTask2Index, taskStartTime);
}
}  // namespace static_scheduling_with_event
