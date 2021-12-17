#include "WF_SDK/WF_SDK.h"  // include all classes and functions

#include <iostream>         // needed for input/output
#include <signal.h>         // needed for keyboard interrupt detection
#include <cmath>            // needed for power function

using namespace std;

// macros
#ifdef _WIN32
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
device_data this_device;
supplies_data supplies_state;

/* ----------------------------------------------------- */

int main(void) {
    // connect to the device
    this_device = device.open();

    // check for connection errors
    device.check_error(this_device.handle);

    /* ----------------------------------------------------- */

    // use instruments here
    // start the positive supply
    supplies_state.name = this_device.name;
    supplies_state.master_state = true;
    supplies_state.state = true;
    supplies_state.voltage = 3.3;
    supplies.switch_(this_device.handle, supplies_state);
    //supplies.switch_variable(this_device.handle, true, true, false, 3.3, 0);

    // set maximum output current
    if (this_device.name == "Digital Discovery" || this_device.name == "Analog Discovery Pro 3X50") {
        static_.set_current(this_device.handle, 16);
    }

    // set all pins as output
    for (int index = 0; index < 16; index++) {
        static_.set_mode(this_device.handle, index, true);
    }

    signal(SIGINT, signal_handler);

    cout << "Press Ctrl+C to exit...\n";

    while(true) {
        // repeat
        int mask = 1;

        while (mask < 0x10000) {
            // go through possible states
            for (int index = 0; index < 16; index++) {
                // set the state of every DIO channel
                bool  state = bool(mask & int(pow(2, index)));
                static_.set_state(this_device.handle, index, !state);
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
                static_.set_state(this_device.handle, index, !state);
            }
            sleep(100);  // delay
        }
    }

    /* ----------------------------------------------------- */

    return 0;
}

/* ----------------------------------------------------- */

void signal_handler(int sig_num) {
    // stop the static I/O
    static_.close(this_device.handle);

    // stop and reset the power supplies
    supplies_state.master_state = false;
    supplies.switch_(this_device.handle, supplies_state);
    supplies.close(this_device.handle);

    // close the connection
    device.close(this_device.handle);

    //cout << "\nPress Enter to exit...";
    //cin.get();
    exit(0);
    return;
}
