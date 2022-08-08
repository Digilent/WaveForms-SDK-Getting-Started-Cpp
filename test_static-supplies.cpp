#include "WF_SDK/WF_SDK.h"  // include all classes and functions
#include <iostream>         // needed for input/output
#include <signal.h>         // needed for keyboard interrupt detection
#include <cmath>            // needed for power function

// macros
#if (defined(_WIN32) || defined(_WIN64))
#   include <windows.h>
#   define sleep(time) Sleep((int)(time))
#else
#   include <unistd.h>
#   include <sys/time.h>
#   define sleep(time) usleep((int)(1000 * time))
#endif

// function prototypes
void signal_handler(int sig_num);

// global variables
Device::Data device_data;
Supplies::Data supplies_data;

/* ----------------------------------------------------- */

int main(void) {
    // connect to the device
    device_data = device.open();

    // check for connection errors
    device.check_error(device_data);

    /* ----------------------------------------------------- */

    // use instruments here
    // start the positive supply
    supplies_data.master_state = true;
    supplies_data.positive_state = true;
    supplies_data.positive_voltage = 3.3;
    supplies.switch_(device_data, supplies_data);
    
    // set all pins as output
    for (int index = 0; index < 16; index++) {
        static_.set_mode(device_data, index, true);
    }

    signal(SIGINT, signal_handler);

    std::cout << "Press Ctrl+C to exit..." << std::endl;

    while(true) {
        // repeat
        int mask = 1;

        while (mask < 0x10000) {
            // go through possible states
            for (int index = 0; index < 16; index++) {
                // set the state of every DIO channel
                bool  state = bool(mask & int(pow(2, index)));
                static_.set_state(device_data, index, !state);
            }
            sleep(100);  // delay
            mask <<= 1;  // switch mask
        }

        while (mask > 1) {
            // go through possible states backward
            mask >>= 1;  // switch mask
            for (int index = 0; index < 16; index++) {
                // set the state of every DIO channel
                bool state = bool(mask & int(pow(2, index)));
                static_.set_state(device_data, index, !state);
            }
            sleep(100);  // delay
        }
    }

    return 0;
}

/* ----------------------------------------------------- */

void signal_handler(int sig_num) {
    // stop the static I/O
    static_.close(device_data);

    // stop and reset the power supplies
    supplies_data.master_state = false;
    supplies.switch_(device_data, supplies_data);
    supplies.close(device_data);

    // close the connection
    device.close(device_data);
    exit(0);
    return;
}
