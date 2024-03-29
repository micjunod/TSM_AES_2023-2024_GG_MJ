// Copyright 2023 GRAC Guilhem JUNOD Mickael

// includes come here
#include "my_candidate_applications.hpp"

#include "mbed_trace.h"
#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "CandidateApplications"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

namespace bike_computer {

uint32_t MyCandidateApplications::getSlotForCandidate() {
    uint32_t numOfSlot = getNbrOfSlots();

    for (uint32_t slotIndex = 0; slotIndex < numOfSlot; slotIndex++) {
        // Check if the slot is not used (doesn't contain a valid candidate application)
        update_client::BlockDeviceApplication& blockdeviceapplication =
            getBlockDeviceApplication(slotIndex);

        if (blockdeviceapplication.isValid() == false) {
            // Return the first unused slot found
            return slotIndex;
        }
    }
    return 0;
}

MyCandidateApplications::MyCandidateApplications(BlockDevice& blockDevice,  // NOLINT
                                                 mbed::bd_addr_t storageAddress,
                                                 mbed::bd_size_t storageSize,
                                                 uint32_t headerSize,
                                                 uint32_t nbrOfSlots)
    : update_client::CandidateApplications(
          blockDevice, storageAddress, storageSize, headerSize, nbrOfSlots) {}

}  // namespace bike_computer

update_client::CandidateApplications* createCandidateApplications(
    BlockDevice& blockDevice,  // NOLINT
    mbed::bd_addr_t storageAddress,
    mbed::bd_size_t storageSize,
    uint32_t headerSize,
    uint32_t nbrOfSlots) {
    return new bike_computer::MyCandidateApplications(
        blockDevice, storageAddress, storageSize, headerSize, nbrOfSlots);
}
