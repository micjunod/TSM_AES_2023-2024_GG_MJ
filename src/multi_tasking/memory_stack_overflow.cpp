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

#include "memory_stack_overflow.hpp"

#include <cstdint>

namespace multi_tasking {

// cppcheck-suppress unusedFunction
void MemoryStackOverflow::allocateOnStack() {
    // allocate an array with growing size until it does not fit on the stack anymore
    size_t allocSize = kArraySize * _multiplier;
    // Create a variable-size object on the stack
    double anotherArray[allocSize];  // NOLINT
    for (size_t i = 0; i < allocSize; i++) {
        anotherArray[i] = i;
    }
    // copy to member variable to prevent them from being optimized away
    for (size_t i = 0; i < kArraySize; i++) {
        // cppcheck-suppress uninitvar
        _doubleArray[i] += anotherArray[i];
    }
    _multiplier++;
}

}  // namespace multi_tasking
