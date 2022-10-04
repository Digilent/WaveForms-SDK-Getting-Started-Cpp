#include "WF_SDK/WF_SDK.h"  // include all classes and functions
#include <iostream>         // needed for input/output
#include <string>           // needed for error handling

using namespace wf;

/* ----------------------------------------------------- */

int main(void) {
    Device::Data *device_data;
    try {
        // connect to the device
        device_data = device.open();

        // use instruments here

        // close the connection
        device.close(device_data);
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
