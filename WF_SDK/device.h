/* DEVICE CONTROL FUNCTIONS: open, check_error, close */

/* include the necessary libraries */
#include <string>
#include <iterator>
#include <map>
#include <iostream>

using namespace std;

/* include the constants and the WaveForms function library */
#ifdef _WIN32
#include "C:/Program Files (x86)/Digilent/WaveFormsSDK/inc/dwf.h"
#elif __APPLE__
#include "/Library/Frameworks/dwf.framework/Headers/dwf.h"
#else
#include <digilent/waveforms/dwf.h>
#endif

/* ----------------------------------------------------- */

class device_data {
    public:
        HDWF handle;
        string name;
        device_data& operator=(const device_data&);
};

device_data& device_data::operator=(const device_data &data) {
    if (this != &data) {
        handle = data.handle;
        name = data.name;
    }
    return *this;
}

/* ----------------------------------------------------- */

class Device {
    public:
        device_data open(string device = "");
        void check_error(HDWF device_handle);
        void close(HDWF device_handle);
} device;
