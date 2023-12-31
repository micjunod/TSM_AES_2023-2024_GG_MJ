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

#include "memory_fragmenter.hpp"

#include "mbed.h"
#include "mbed_trace.h"
#include "memory_logger.hpp"
#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "Fragmenter"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

namespace multi_tasking {

// create a memory leak in the constructor itself
MemoryFragmenter::MemoryFragmenter() {}

// cppcheck-suppress unusedFunction
void MemoryFragmenter::fragmentMemory() {
    // create a memory logger
    advembsof::MemoryLogger memorLogger;

    // get heap info
    mbed_stats_heap_t heapInfo = {0};
    mbed_stats_heap_get(&heapInfo);
    uint32_t availableSize =
        heapInfo.reserved_size - heapInfo.current_size - heapInfo.overhead_size;
    tr_debug("Available heap size is %" PRIu32 " (reserved %" PRIu32 ")",
             availableSize,
             heapInfo.reserved_size);

    // divide the available size by 8 blocks that we allocate
    uint32_t blockSize = (availableSize - kMarginSpace) / kNbrOfBlocks;
    tr_debug("Allocating blocks of size %" PRIu32 "", blockSize);
    char* pBlockArray[kNbrOfBlocks] = {NULL};
    for (uint32_t blockIndex = 0; blockIndex < kNbrOfBlocks; blockIndex++) {
        pBlockArray[blockIndex] = new char[blockSize];
        if (pBlockArray[blockIndex] == NULL) {
            tr_error("Cannot allocate block memory for index %" PRIu32 "", blockIndex);
        }
        tr_debug("Allocated block index  %" PRIu32 " of size  %" PRIu32
                 " at address 0x%08" PRIx32 "",
                 blockIndex,
                 blockSize,
                 (uint32_t)pBlockArray[blockIndex]);
        // copy to member variable to prevent them from being optimized away
        for (uint32_t index = 0; index < kArraySize; index++) {
            _doubleArray[index] += (double)pBlockArray[blockIndex][index];  // NOLINT
        }
    }
    // the full heap (or almost) should be allocated
    tr_debug("Heap statistics after full allocation:");
    memorLogger.getAndPrintHeapStatistics();
    // delete only the even blocks
    for (uint32_t blockIndex = 0; blockIndex < kNbrOfBlocks; blockIndex += 2) {
        delete[] pBlockArray[blockIndex];
        pBlockArray[blockIndex] = NULL;
    }
    // we should have half of the heap space free
    tr_debug("Heap statistics after half deallocation:");
    memorLogger.getAndPrintHeapStatistics();

    // trying to allocated one block that is slightly bigger
    // without fragmentation, this allocation should succeed
    heapInfo = {0};
    mbed_stats_heap_get(&heapInfo);
    availableSize =
        heapInfo.reserved_size - heapInfo.current_size - heapInfo.overhead_size;
    tr_debug("Available heap size is  %" PRIu32 " (reserved  %" PRIu32 ")",
             availableSize,
             heapInfo.reserved_size);
    blockSize += 8;
    // this allocation will fail
    tr_debug("Allocating 1 block of size %" PRIu32 " should succeed !", blockSize);
    pBlockArray[0] = new char[blockSize];
    // copy to member variable to prevent them from being optimized away
    for (uint32_t index = 0; index < kArraySize; index++) {
        _doubleArray[index] += (double)pBlockArray[0][index];  // NOLINT
    }
}

}  // namespace multi_tasking
