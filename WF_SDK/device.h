/* DEVICE CONTROL FUNCTIONS: open, check_error, close */

/* include the necessary libraries */
#include <string>
#include <iterator>
#include <map>
#include <iostream>

using namespace std;

/* include the constants and the WaveForms function library */
#ifdef WIN32
#include "C:/Program Files (x86)/Digilent/WaveFormsSDK/inc/dwf.h"
#elif __APPLE__
#include "/Library/Frameworks/dwf.framework/Headers/dwf.h"
#else
#include <digilent/waveforms/dwf.h>
#endif

/* ----------------------------------------------------- */

class device {
    public:
        HDWF open(string* device_name, string device = "");
        void check_error(HDWF device_handle);
        void close(HDWF device_handle);
};
