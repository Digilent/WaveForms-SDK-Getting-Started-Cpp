/* DEVICE CONTROL FUNCTIONS: open, check_error, close */

/* include the header */
#include "device.h"

/* ----------------------------------------------------- */

device_data Device::open(string device) {
    /*
        open a specific device

        parameters: - device type: None (first device), "Analog Discovery", "Analog Discovery 2", "Analog Discovery Studio", "Digital Discovery", "Analog Discovery Pro 3X50", "Analog Discovery Pro 5250"
        
        returns:    - the device handle
                    - the device name
    */
    device_data dev;

    map<string, int> device_names;
        device_names["Analog Discovery"] = devidDiscovery;
        device_names["Analog Discovery 2"] = devidDiscovery2;
        device_names["Analog Discovery Studio"] = devidDiscovery2;
        device_names["Digital Discovery"] = devidDDiscovery;
        device_names["Analog Discovery Pro 3X50"] = devidADP3X50;
        device_names["Analog Discovery Pro 5250"] = devidADP5250;

    // decode device names
    ENUMFILTER device_type = enumfilterAll;
    for(map<string, int>::iterator pair = device_names.begin(); pair != device_names.end(); ++pair) {
        if (device == pair -> first) {
            device_type = pair -> second;
            break;
        }
    }

    // count devices
    int device_count = 0;
    FDwfEnum(device_type, &device_count);

    // check for connected devices
    if (device_count <= 0) {
        if (device_type == 0) {
            cout << "Error: There are no connected devices" << endl;
        }
        else {
            cout << "Error: There is no " << device << " connected" << endl;
        }
        throw exception();
    }

    // this is the device handle - it will be used by all functions to "address" the connected device
    dev.handle = 0;

    // connect to the first available device
    HDWF index = 0;
    while (dev.handle == 0 && index < device_count) {
        FDwfDeviceOpen(index, &dev.handle);
        index++;    // increment the index and try again if the device is busy
    }

    // check connected device type
    dev.name = "";
    if (dev.handle != 0) {
        int device_id = 0;
        int device_rev = 0;
        FDwfEnumDeviceType(index - 1, &device_id, &device_rev);

        // decode device id
        for(map<string, int>::iterator pair = device_names.begin(); pair != device_names.end(); ++pair) {
            if (device_id == pair -> second) {
                dev.name = pair -> first;
                break;
            }
        }
    }
    return dev;
}

/* ----------------------------------------------------- */

void Device::check_error(HDWF device_handle) {
    /*
        check for connection errors
    */
    // if the device handle is empty after a connection attempt
    if (device_handle == hdwfNone) {
        // check for errors
        int err_nr = 0; // variable for error number
        FDwfGetLastError(&err_nr);  // get error number
    
        // if there is an error
        if (err_nr != dwfercNoErc) {
            // display it and quit
            char err_msg[512];  // variable for the error message
            FDwfGetLastErrorMsg(err_msg);  // get the error message
            cout << "Error: " << err_msg << endl;   // display error message
            throw exception();  // exit the program
        }
    }
    return;
}

/* ----------------------------------------------------- */

void Device::close(HDWF device_handle) {
    /*
        close a specific device
    */
    FDwfDeviceClose(device_handle);
    return;
}
