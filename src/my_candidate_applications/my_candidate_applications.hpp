// Copyright 2023 GRAC Guilhem JUNOD Mickael

// includes come here
#pragma once

#include "candidate_applications.hpp"

namespace bike_computer {

    class MyCandidateApplications : public update_client::CandidateApplications
    {
        public:
            uint32_t getSlotForCandidate() override;

            MyCandidateApplications(BlockDevice& blockDevice,
                          mbed::bd_addr_t storageAddress,
                          mbed::bd_size_t storageSize,
                          uint32_t headerSize,
                          uint32_t nbrOfSlots);
    };

    
}

update_client::CandidateApplications* createCandidateApplications(
    BlockDevice& blockDevice,
    mbed::bd_addr_t storageAddress,
    mbed::bd_size_t storageSize,
    uint32_t headerSize,
    uint32_t nbrOfSlots);

