#include "WF_SDK/WF_SDK.h"  // include all classes and functions

/* ----------------------------------------------------- */

void main(void) {
    // connect to the device
    device_data this_device;
    this_device = device.open();

    // check for connection errors
    device.check_error(this_device.handle);

    /* ----------------------------------------------------- */

    // use instruments here


    /* ----------------------------------------------------- */

    // close the connection
    device.close(this_device.handle);
    return;
}
