#include "WF_SDK/WF_SDK.h"  // include all classes and functions
#include <iostream>         // needed for input/output
#include <cmath>            // needed for ceil
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

        tools.keyboard_interrupt_reset(device_data);

        std::cout << "Press Ctrl+C to exit..." << std::endl;

        while (true) {
            double temp = device.temperature(device_data);  // get board temperature
            std::cout << "board temperature: " << std::ceil(temp * 100.0) / 100.0 << "C" << std::endl;  // display temperature
            tools.sleep(500);
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
