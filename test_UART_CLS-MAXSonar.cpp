#include "WF_SDK/WF_SDK.h"  // include all classes and functions

#include <iostream>         // needed for input/output
#include <signal.h>         // needed for keyboard interrupt handling
#include <sstream>          // needed for string manipulation

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
uart_data output_data;

int reset = 2;      // define MAXSonar reset line
int timeout = 1000; // define timeout iteration count

/* ----------------------------------------------------- */

int main(void) {
    // connect to the device
    device_data this_device;
    this_device = device.open();

    // check for connection errors
    device.check_error(this_device.handle);

    /* ----------------------------------------------------- */

    // start the power supplies
    supplies_state.name = this_device.name;
    supplies_state.master_state = true;
    supplies_state.state = true;
    supplies_state.voltage = 3.3;
    supplies.switch_(this_device.handle, supplies_state);
    sleep(100);    // delay

    // initialize the reset line
    static_.set_mode(this_device.handle, reset, true);
    static_.set_state(this_device.handle, reset, false);

    // initialize the uart interface on DIO0 and DIO1
    uart.open(this_device.handle, 1, 0, 9600);

    signal(SIGINT, signal_handler);

    cout << "Press Ctrl+C to exit...\n";

    while (true) {
        // repeat
        // clear the screen and home cursor
        uart.write(this_device.handle, "\x1b[j");

        // display a message
        uart.write(this_device.handle, "Dist: ");

        // read raw data
        static_.set_state(this_device.handle, reset, true);    // enable the device
        int current_timeout = timeout;
        while (current_timeout > 0) {
            // wait for data
            output_data = uart.read(this_device.handle);
            if (output_data.data.data() != "") {
                // exit when data is received
                break;
            }
            current_timeout--;
        }
        static_.set_state(this_device.handle, reset, false);    // disable the device

        // convert raw data into distance
        int value = 0;
        if (output_data.data[0] == 234) {
            for (int index = 1; index < output_data.data.size(); index++) {
                if (output_data.data[index] > 47 && output_data.data[index] < 58) {
                    // concatenate valid bytes
                    value = value * 10 + (output_data.data[index] - 48);
                }
            }
            value *= 2.54;   // convert to cm
        }
        else {
            // error in message
            value = -1;
        }

        // display the distance
        ostringstream output;
        output.precision(2);
        output << fixed << value;
        uart.write(this_device.handle, output.str());

        // display a message
        uart.write(this_device.handle, "cm");

        // delay 1s
        sleep(1000);
    }

    /* ----------------------------------------------------- */

    return 0;
}

/* ----------------------------------------------------- */

void signal_handler(int sig_num) {
    // reset the interface
    uart.close(this_device.handle);

    // reset the static I/O
    static_.set_mode(this_device.handle, reset, false);
    static_.set_state(this_device.handle, reset, true);
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
