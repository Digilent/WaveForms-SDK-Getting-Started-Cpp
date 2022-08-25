/* POWER SUPPLIES CONTROL FUNCTIONS: switch, close */

/* include the necessary libraries */
#include <string>
#include <algorithm>
#include "dwf.h"
#include "device.h"

#ifndef WF_SUPPLIES
#define WF_SUPPLIES
namespace wf {

class Supplies {
    private:
        class State {
            public:
                bool on = false;
                bool off = true;
        };
        double min(double a, double b);
        double max(double a, double b);
    public:
        class Data {
            public:
                bool master_state = false;
                bool positive_state = false;
                bool negative_state = false;
                bool state = false;
                double positive_voltage = 0;
                double negative_voltage = 0;
                double voltage = 0;
                double positive_current = 0;
                double negative_current = 0;
                double current = 0;
                Data& operator=(const Data &data) {
                    if (this != &data) {
                        master_state = data.master_state;
                        positive_state = data.positive_state;
                        negative_state = data.negative_state;
                        state = data.state;
                        positive_voltage = data.positive_voltage;
                        negative_voltage = data.negative_voltage;
                        voltage = data.voltage;
                        positive_current = data.positive_current;
                        negative_current = data.negative_current;
                        current = data.current;
                    }
                    return *this;
                }
        };
        State state;
        void switch_(Device::Data device_data, Data supplies_data);
        void close(Device::Data device_data);
} supplies;

}
#endif
