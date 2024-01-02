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

#include "bike_system.hpp"

#include <chrono>

#include "mbed_trace.h"
#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "BikeSystem"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

#include "memory_fragmenter.hpp"
#include "memory_leak.hpp"
#include "memory_stack_overflow.hpp"

namespace multi_tasking {

static constexpr std::chrono::milliseconds kGearTaskPeriod                   = 800ms;
static constexpr std::chrono::milliseconds kGearTaskDelay                    = 0ms;
static constexpr std::chrono::milliseconds kGearTaskComputationTime          = 100ms;
static constexpr std::chrono::milliseconds kSpeedDistanceTaskPeriod          = 400ms;
static constexpr std::chrono::milliseconds kSpeedDistanceTaskDelay           = 0ms;
static constexpr std::chrono::milliseconds kSpeedDistanceTaskComputationTime = 200ms;
static constexpr std::chrono::milliseconds kDisplayTaskPeriod                = 1600ms;
static constexpr std::chrono::milliseconds kDisplayTaskDelay                 = 300ms;
static constexpr std::chrono::milliseconds kDisplayTaskComputationTime       = 200ms;
static constexpr std::chrono::milliseconds kResetTaskPeriod                  = 800ms;
static constexpr std::chrono::milliseconds kResetTaskDelay                   = 700ms;
static constexpr std::chrono::milliseconds kResetTaskComputationTime         = 100ms;
static constexpr std::chrono::milliseconds kTemperatureTaskPeriod            = 1600ms;
static constexpr std::chrono::milliseconds kTemperatureTaskDelay             = 1100ms;
static constexpr std::chrono::milliseconds kTemperatureTaskComputationTime   = 100ms;
static constexpr std::chrono::milliseconds kMajorCycleDuration               = 1600ms;

BikeSystem::BikeSystem()
    : _thread(osPriorityNormal, OS_STACK_SIZE, nullptr, "ISR_Thread"),
      _resetDevice(callback(this, &BikeSystem::onReset)),
      _displayDevice(),
      _speedometer(_timer),
      _sensorDevice(),
      _gearDevice(callback(this, &BikeSystem::onUp), callback(this, &BikeSystem::onDown)),
      _pedalDevice(callback(this, &BikeSystem::onLeft),
                   callback(this, &BikeSystem::onRight)),
      _cpuLogger(_timer) {}

// int otherStackOverflow(int i) {
//     tr_warn("%d", i);
//     return otherStackOverflow(i + 1);
// }

void BikeSystem::start() {
    tr_info("Starting multi tasking");

    // Used to verify the heap fragmentation error
    // MemoryFragmenter fragmenter;
    // fragmenter.fragmentMemory();

    // Used to verify the stack overflow error
    // MemoryStackOverflow overflow;
    // Thread stackThread(osPriorityNormal, OS_STACK_SIZE, nullptr,
    // "Stack_Overflow_Thread"); EventQueue eventQoverflow;
    // stackThread.start(callback(&eventQoverflow, &EventQueue::dispatch_forever));
    // Event<void()> overflowEvent(&eventQoverflow, callback(&overflow,
    // &MemoryStackOverflow::allocateOnStack)); overflowEvent.period(1);
    // overflowEvent.post();

    // Custom function to realize a stack overflow
    // otherStackOverflow(1);

    init();

    osStatus status =
        _thread.start(callback(&_eventQueueISR, &EventQueue::dispatch_forever));
    tr_debug("Thread %s started with status %d", _thread.get_name(), status);

    _memoryLogger.getAndPrintStatistics();

    /*
    Ya deux Queue : une pr le thread main pour lancer les taches périodiques (deja fait)
    et une pr le thread qui execute les task ISR Qd ya un ISR, on vient remplir la Queue
    de _thread avec un event La Queue de _thread est en dispatch foreever
    */

    EventQueue eventQueuePeriodic;

    Event<void()> temperatureEvent(&eventQueuePeriodic,
                                   callback(this, &BikeSystem::temperatureTask));
    temperatureEvent.delay(kTemperatureTaskDelay);
    temperatureEvent.period(kTemperatureTaskPeriod);
    temperatureEvent.post();

    Event<void()> displayEvent(&eventQueuePeriodic,
                               callback(this, &BikeSystem::displayTask));
    displayEvent.delay(kDisplayTaskDelay);
    displayEvent.period(kDisplayTaskPeriod);
    displayEvent.post();

#if !MBED_TEST_MODE
    // event for dynamic memory profiling
    Event<void()> memoryStatsEvent(
        &eventQueuePeriodic,
        callback(&_memoryLogger, &advembsof::MemoryLogger::printDiffs));
    memoryStatsEvent.delay(kMajorCycleDuration);
    memoryStatsEvent.period(kMajorCycleDuration);
    memoryStatsEvent.post();

    // legacy event printing cpu stats
    /*Event<void()> cpuStatsEvent(&eventQueuePeriodic,
                                callback(&_cpuLogger, &advembsof::CPULogger::printStats));
    cpuStatsEvent.delay(kMajorCycleDuration);
    cpuStatsEvent.period(kMajorCycleDuration);
    cpuStatsEvent.post();*/
#endif

    eventQueuePeriodic.dispatch_forever();
}

void BikeSystem::stop() { core_util_atomic_store_bool(&_stopFlag, true); }

#if defined(MBED_TEST_MODE)
const advembsof::TaskLogger& BikeSystem::getTaskLogger() { return _taskLogger; }
bike_computer::Speedometer& BikeSystem::getSpeedometer() { return _speedometer; }
const uint8_t BikeSystem::getCurrentGear() { return _currentGear; }
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

void BikeSystem::onUp() {
    _gearDevice.incrementGear();
    Event<void()> gearEvent(&_eventQueueISR, callback(this, &BikeSystem::gearTask));
    gearEvent.post();
}

void BikeSystem::onDown() {
    _gearDevice.decrementGear();
    Event<void()> gearEvent(&_eventQueueISR, callback(this, &BikeSystem::gearTask));
    gearEvent.post();
}

void BikeSystem::gearTask() {
    // need to protect access to data members (multi threaded)
    core_util_atomic_store_u8(&_currentGear, _gearDevice.getCurrentGear());
    core_util_atomic_store_u8(&_currentGearSize, _gearDevice.getCurrentGearSize());
}

void BikeSystem::onLeft() {
    _pedalDevice.incrementPedal();
    Event<void()> speedDistanceEvent(&_eventQueueISR,
                                     callback(this, &BikeSystem::speedDistanceTask));
    speedDistanceEvent.post();
}

void BikeSystem::onRight() {
    _pedalDevice.decrementPedal();
    Event<void()> speedDistanceEvent(&_eventQueueISR,
                                     callback(this, &BikeSystem::speedDistanceTask));
    speedDistanceEvent.post();
}

void BikeSystem::speedDistanceTask() {
    // need to protect access to data members (multi threaded)
    // TODO(truc): atomic
    const auto pedalRotationTime = _pedalDevice.getCurrentRotationTime();
    _speedometer.setCurrentRotationTime(pedalRotationTime);
    _speedometer.setGearSize(core_util_atomic_load_u8(&_currentGearSize));

    _currentSpeed = _speedometer.getCurrentSpeed();
}

void BikeSystem::temperatureTask() {
    // Used to verify the memory leak error
    // MemoryLeak* leak = new MemoryLeak();
    // leak->use();

    auto taskStartTime = _timer.elapsed_time();

    // no need to protect access to data members (single threaded)
    _currentTemperature = _sensorDevice.readTemperature();

    auto elapsedTimeTask = std::chrono::duration_cast<std::chrono::milliseconds>(
        _timer.elapsed_time() - taskStartTime);

    _taskLogger.logPeriodAndExecutionTime(
        _timer, advembsof::TaskLogger::kTemperatureTaskIndex, taskStartTime);
}

void BikeSystem::onReset() {
    Event<void()> resetEvent(&_eventQueueISR, callback(this, &BikeSystem::resetTask));
    resetEvent.post();
}

void BikeSystem::resetTask() { _speedometer.reset(); }

void BikeSystem::displayTask() {
    auto taskStartTime = _timer.elapsed_time();

    _traveledDistance = _speedometer.getDistance();
    _displayDevice.displayGear(_currentGear);
    _displayDevice.displaySpeed(_currentSpeed);
    _displayDevice.displayDistance(_traveledDistance);
    _displayDevice.displayTemperature(_currentTemperature);

    auto elapsedTimeTask = std::chrono::duration_cast<std::chrono::milliseconds>(
        _timer.elapsed_time() - taskStartTime);

    _taskLogger.logPeriodAndExecutionTime(
        _timer, advembsof::TaskLogger::kDisplayTask1Index, taskStartTime);
}

}  // namespace multi_tasking
