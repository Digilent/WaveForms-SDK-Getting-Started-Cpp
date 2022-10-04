/* DIGITAL MULTIMETER CONTROL FUNCTIONS: open, measure, close */

/* include the necessary libraries */
#include <string>
#include "dwf.h"
#include "device.h"

#ifndef WF_DMM
#define WF_DMM
namespace wf {

class DMM {
    private:
        class Mode {
            public:
                const DwfDmm ac_voltage = DwfDmmACVoltage;
                const DwfDmm dc_voltage = DwfDmmDCVoltage;
                const DwfDmm ac_high_current = DwfDmmACCurrent;
                const DwfDmm dc_high_current = DwfDmmDCCurrent;
                const DwfDmm ac_low_current = DwfDmmACLowCurrent;
                const DwfDmm dc_low_current = DwfDmmDCLowCurrent;
                const DwfDmm resistance = DwfDmmResistance;
                const DwfDmm continuity = DwfDmmContinuity;
                const DwfDmm diode = DwfDmmDiode;
                const DwfDmm temperature = DwfDmmTemperature;
        };

        class Data {
            private:
                class Nodes {
                    public:
                        int enable = -1;
                        int mode = -1;
                        int range = -1;
                        int meas = -1;
                        int raw = -1;
                        int input = -1;
                        Nodes& operator=(const Nodes &data) {
                            if (this != &data) {
                                enable = data.enable;
                                mode = data.mode;
                                range = data.range;
                                meas = data.meas;
                                raw = data.raw;
                                input = data.input;
                            }
                            return *this;
                        }
                };
            public:
                int channel = -1;
                DwfDmm mode = -1;
                Nodes nodes;
                Data& operator=(const Data &data) {
                    if (this != &data) {
                        channel = data.channel;
                        mode = data.mode;
                        nodes = data.nodes;
                    }
                    return *this;
                }
        };
    public:
        Mode mode;
        Data data;
        void open(Device::Data *device_data);
        double measure(Device::Data *device_data, DwfDmm mode, double range = 0, bool high_impedance = false);
        void close(Device::Data *device_data);
} dmm;

}
#endif
