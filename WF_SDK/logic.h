/* LOGIC ANALYZER CONTROL FUNCTIONS: open, trigger, record, close */

/* include the necessary libraries */
#include <algorithm>

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

class logic_data {
    public:
        unsigned char* buffer;
        double* time;
        logic_data& operator=(const logic_data&);
};

logic_data& logic_data::operator=(const logic_data &data) {
    if (this != &data) {
        buffer = data.buffer;
        time = data.time;
    }
    return *this;
}

/* ----------------------------------------------------- */

class Logic {
    public:
        void open(HDWF device_handle, double sampling_frequency = 100e06, int buffer_size = 4096);
        void trigger(HDWF device_handle, bool enable, int channel, int buffer_size = 4096, int position = 0, double timeout = 0, bool rising_edge = true, double length_min = 0, double length_max = 20, int count = 1);
        logic_data record(HDWF device_handle, int channel, double sampling_frequency = 100e06, int buffer_size = 4096);
        void close(HDWF device_handle);
} logic;
