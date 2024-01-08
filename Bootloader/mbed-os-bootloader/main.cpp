#include "mbed.h"

#include "mbed_trace.h"
#include "FlashIAPBlockDevice.h"
#include "update-client/uc_error_code.hpp"
#include "update-client/block_device_application.hpp"
#include "candidate_applications.hpp"
#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "bootloader"
#endif // MBED_CONF_MBED_TRACE_ENABLE

#if MBED_CONF_MBED_TRACE_ENABLE
static UnbufferedSerial g_uart(CONSOLE_TX, CONSOLE_RX);

// Function that directly outputs to an unbuffered serial port in blocking mode.
static void boot_debug(const char *s)
{
    size_t len = strlen(s);
    g_uart.write(s, len);
    g_uart.write("\r\n", 2);
}
#endif

int main()
{
#if MBED_CONF_MBED_TRACE_ENABLE
    mbed_trace_init();
    mbed_trace_print_function_set(boot_debug);
#endif // MBED_CONF_MBED_TRACE_ENABLE

        //mbed::bd_addr_t applicationAddress = APPLICATION_ADDR - MBED_ROM_START;
        mbed::bd_addr_t applicationAddress = 0x20080;

        //mbed::bd_addr_t storageAddress = HEADER_ADDR - MBED_ROM_START;
        mbed::bd_addr_t storageAddress = 0x20000;
        
        mbed::bd_size_t storageSize = MBED_ROM_SIZE/2;
        uint32_t headerSize = HEADER_SIZE;
        uint32_t nbrOfSlots = MBED_CONF_UPDATE_CLIENT_STORAGE_LOCATIONS;
        uint32_t newestSlotIndex = 0;

        FlashIAPBlockDevice flashIAPBlockDevice(MBED_ROM_START, MBED_ROM_SIZE);
        flashIAPBlockDevice.init();

        update_client::BlockDeviceApplication blockDeviceApplication(flashIAPBlockDevice, storageAddress, applicationAddress);
        tr_debug("Application Address : 0x%10llx \r\n", applicationAddress);
        update_client::UCErrorCode error = blockDeviceApplication.checkApplication();

        
        update_client::CandidateApplications candidateApplications(flashIAPBlockDevice, storageAddress, storageSize, headerSize, nbrOfSlots);

    
        if(candidateApplications.hasValidNewerApplication(blockDeviceApplication,newestSlotIndex))
        {
            error = candidateApplications.installApplication(newestSlotIndex,storageAddress);
        }

        


    tr_debug("BikeComputer bootloader\r\n");

    // at this stage we directly branch to the main application
    void *sp = *((void **) POST_APPLICATION_ADDR + 0);  // NOLINT(readability/casting)
    void *pc = *((void **) POST_APPLICATION_ADDR + 1);  // NOLINT(readability/casting)
    tr_debug("Starting application at address 0x%08x (sp 0x%08x, pc 0x%08x)\r\n", POST_APPLICATION_ADDR, (uint32_t) sp, (uint32_t) pc);

    mbed_start_application(POST_APPLICATION_ADDR);

    return 0;
}
