/* POWER SUPPLIES CONTROL FUNCTIONS: switch, switch_fixed, switch_variable, switch_digital, switch_6V, switch_25V, close */

/* include the necessary libraries */
#include <string>
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

class supplies_data {
    public:
        string name;
        int master_state;
        int state = -1;
        int positive_state = -1;
        int negative_state = -1;
        double voltage = -1;
        double positive_voltage = -1;
        double negative_voltage = 1;
        double current = -1;
        double positive_current = -1;
        double negative_current = 1;
        supplies_data& operator=(const supplies_data&);
};

supplies_data& supplies_data::operator=(const supplies_data &data) {
    if (this != &data) {
        name = data.name;
        state = data.state;
        positive_state = data.positive_state;
        negative_state = data.negative_state;
        voltage = data.voltage;
        positive_voltage = data.positive_voltage;
        negative_voltage = data.negative_voltage;
        current = data.current;
        positive_current = data.positive_current;
        negative_current = data.negative_current;
    }
    return *this;
}

/* ----------------------------------------------------- */

class Supplies {
    public:
        void switch_fixed(HDWF device_handle, int master_state, int positive_state, int negative_state);
        void switch_variable(HDWF device_handle, int master_state, int positive_state, int negative_state, double positive_voltage, double negative_voltage);
        void switch_digital(HDWF device_handle, int master_state, double voltage);
        void switch_6V(HDWF device_handle, int master_state, double voltage, double current = 1);
        void switch_25V(HDWF device_handle, int positive_state, int negative_state, double positive_voltage, double negative_voltage, double positive_current = 0.5, double negative_current = -0.5);
        bool switch_(HDWF device_handle, supplies_data state);
        void close(HDWF device_handle);
} supplies;
