/* STATIC I/O CONTROL FUNCTIONS: set_mode, get_state, set_state, set_current, set_pull, close */

/* include the necessary libraries */
#include <intrin.h>
#include <algorithm>
#include <math.h>

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

class Static {
    public:
        void set_mode(HDWF device_handle, int channel, bool output);
        bool get_state(HDWF device_handle, int channel);
        void set_state(HDWF device_handle, int channel, bool state);
        void set_current(HDWF device_handle, int current);
        void set_pull(HDWF device_handle, int channel, bool direction = bool(-1));
        void close(HDWF device_handle);
} static_;
