/* PROTOCOL: I2C CONTROL FUNCTIONS: open, read, write, exchange, spy, close */

/* include the necessary libraries */
#include <string>
#include <string.h>
#include <vector>

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

class i2c_data {
    public:
        vector<unsigned char> data;
        string error;
        int address;
        string start;
        string stop;
        string direction;
        i2c_data& operator=(const i2c_data&);
};

i2c_data& i2c_data::operator=(const i2c_data &data) {
    if (this != &data) {
        error = data.error;
        address = data.address;
        start = data.start;
        stop = data.stop;
        direction = data.direction;
    }
    return *this;
}

/* ----------------------------------------------------- */

class I2C {
    public:
        string open(HDWF device_handle, int sda, int scl, double clk_rate = 100e03, bool stretching = true);
        i2c_data read(HDWF device_handle, int count, int address);
        string write(HDWF device_handle, string data, int address);
        string write(HDWF device_handle, vector<unsigned char> data, int address);
        i2c_data exchange(HDWF device_handle, string data, int count, int address);
        i2c_data exchange(HDWF device_handle, vector<unsigned char> data, int count, int address);
        i2c_data spy(HDWF device_handle, int count = 16);
        void close(HDWF device_handle);
} i2c;
