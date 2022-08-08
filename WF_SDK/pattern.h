/* PATTERN GENERATOR CONTROL FUNCTIONS: generate, close, enable, disable */

/* include the necessary libraries */
#include <math.h>
#include <vector>
#include "dwf.h"
#include "device.h"

#ifndef WF_PATTERN
#define WF_PATTERN

class Pattern {
    private:
        class Function {
            /* function names */
            public:
                const DwfDigitalOutType pulse = DwfDigitalOutTypePulse;
                const DwfDigitalOutType custom = DwfDigitalOutTypeCustom;
                const DwfDigitalOutType random = DwfDigitalOutTypeRandom;
        };

        class Trigger_Source {
            /* trigger source names */
            public:
                const TRIGSRC none = trigsrcNone;
                const TRIGSRC analog = trigsrcDetectorAnalogIn;
                const TRIGSRC digital = trigsrcDetectorDigitalIn;
                const TRIGSRC external[5] = {trigsrcNone, trigsrcExternal1, trigsrcExternal2, trigsrcExternal3, trigsrcExternal4};
        };

        class Idle_State {
            /* idle states */
            public:
                int initial = DwfDigitalOutIdleInit;
                int high = DwfDigitalOutIdleHigh;
                int low = DwfDigitalOutIdleLow;
                int high_impedance = DwfDigitalOutIdleZet;
        };

        class State {
            public:
                bool on = false;
                bool off = true;
                std::vector<bool> channel{false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
                State& operator=(const State &data) {
                    if (this != &data) {
                        on = data.on;
                        off = data.off;
                        channel = data.channel;
                    }
                    return *this;
                }
        };

    public:
        Function function;
        Trigger_Source trigger_source;
        Idle_State idle_state;
        State state;
        void generate(Device::Data device_data, int channel, DwfDigitalOutType function, double frequency, double duty_cycle = 50.0, std::vector<unsigned short> data = std::vector<unsigned short>(), double wait = 0, int repeat = 0, int run_time = 0, DwfDigitalOutIdle idle = DwfDigitalOutIdleInit, bool trigger_enabled = false, TRIGSRC trigger_source = trigsrcNone, bool trigger_edge_rising = true);
        void close(Device::Data device_data);
        void enable(Device::Data device_data, int channel);
        void disable(Device::Data device_data, int channel);
} pattern;

#endif
