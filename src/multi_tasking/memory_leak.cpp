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

#include "memory_leak.hpp"

namespace multi_tasking {

// create a memory leak in the constructor itself
// cppcheck-suppress [noCopyConstructor,noOperatorEq,noDestructor]
MemoryLeak::MemoryLeak() { _ptr = new int[kArraySize]; }

// cppcheck-suppress unusedFunction
void MemoryLeak::use() {
    for (uint16_t i = 0; i < kArraySize; i++) {
        _ptr[i] = i;
    }
}

}  // namespace multi_tasking
