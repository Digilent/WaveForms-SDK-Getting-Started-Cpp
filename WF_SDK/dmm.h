/* DIGITAL MULTIMETER CONTROL FUNCTIONS: open, measure, close */

/* include the necessary libraries */
#include <string>

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

class DMM {
    public:
        void open(HDWF device_handle);
        double measure(HDWF device_handle, string mode, bool ac = false, double range = 0, bool high_impedance = false);
        void close(HDWF device_handle);
} dmm;
