/* POWER SUPPLIES CONTROL FUNCTIONS: switch, switch_fixed, switch_variable, switch_digital, switch_6V, switch_25V, close */

/* include the necessary libraries */
#include <string>
#include <algorithm>

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

struct Supplies_Data {
    string name;
    bool master_state;
    bool state = -1;
    bool positive_state = -1;
    bool negative_state = -1;
    double voltage = -1;
    double positive_voltage = -1;
    double negative_voltage = 1;
    double current = -1;
    double positive_current = -1;
    double negative_current = 1;
};

typedef struct Supplies_Data supplies_data;

/* ----------------------------------------------------- */

class Supplies {
    public:
        void switch_fixed(HDWF device_handle, bool master_state, bool positive_state, bool negative_state);
        void switch_variable(HDWF device_handle, bool master_state, bool positive_state, bool negative_state, double positive_voltage, double negative_voltage);
        void switch_digital(HDWF device_handle, bool master_state, double voltage);
        void switch_6V(HDWF device_handle, bool master_state, double voltage, double current = 1);
        void switch_25V(HDWF device_handle, bool positive_state, bool negative_state, double positive_voltage, double negative_voltage, double positive_current = 0.5, double negative_current = -0.5);
        bool switch_(HDWF device_handle, supplies_data state);
        void close(HDWF device_handle);
} supplies;
