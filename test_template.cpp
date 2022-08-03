#include "WF_SDK/WF_SDK.h"  // include all classes and functions
#include <iostream>         // needed for input/output

/* ----------------------------------------------------- */

int main(void) {
    // connect to the device
    Device::Data device_data = device.open();

    // check for connection errors
    device.check_error(device_data);

    /* ----------------------------------------------------- */

    // use instruments here

    /* ----------------------------------------------------- */

    // close the connection
    device.close(device_data);

    std::cout << "\nPress Enter to exit...";
    std::cin.get();
    return 0;
}
