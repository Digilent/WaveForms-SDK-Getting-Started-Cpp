#include "WF_SDK/WF_SDK.h"  // include all classes and functions
#include <iostream>         // needed for input/output
#include <cmath>            // needed for power function
#include <string>           // needed for error handling

using namespace wf;

int main(void) {
    // connect to the device
    Device::Data *device_data;
    try {
        device_data = device.open();

        // check for connection errors
        device.check_error(device_data);

        /* ----------------------------------------------------- */

        // use instruments here
        // start the positive supply
        Supplies::Data supplies_data;
        supplies_data.master_state = true;
        supplies_data.positive_state = true;
        supplies_data.positive_voltage = 3.3;
        supplies.switch_(device_data, supplies_data);
        
        // set all pins as output
        for (int index = 0; index < 16; index++) {
            static_.set_mode(device_data, index, true);
        }

        tools.keyboard_interrupt_reset(device_data);

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
                tools.sleep(100);  // delay
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
                tools.sleep(100);  // delay
            }
        }
    }

    catch (Error error) {
        // if an error occurs display it
        std::cout << "Error: ";
        std::cout << error.instrument << " -> ";
        std::cout << error.function << " -> ";
        std::cout << error.message << std::endl;
        // close the connection
        device.close(device_data);
    }
    return 0;
}
