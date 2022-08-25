/* OSCILLOSCOPE CONTROL FUNCTIONS: open, measure, trigger, record, close */

/* include the necessary libraries */
#include <vector>
#include "dwf.h"
#include "device.h"

#ifndef WF_SCOPE
#define WF_SCOPE
namespace wf {

class Scope {
    private:
        class Trigger_Source {
            /* trigger source names */
            public:
                const TRIGSRC none = trigsrcNone;
                const TRIGSRC analog = trigsrcDetectorAnalogIn;
                const TRIGSRC digital = trigsrcDetectorDigitalIn;
                const TRIGSRC external[5] = {trigsrcNone, trigsrcExternal1, trigsrcExternal2, trigsrcExternal3, trigsrcExternal4};
        };

        class Data {
            public:
                int sampling_frequency = 20e06;
                int buffer_size = 0;
                int max_buffer_size = 0;
                Data& operator=(const Data &data) {
                    if (this != &data) {
                        sampling_frequency = data.sampling_frequency;
                        buffer_size = data.buffer_size;
                        max_buffer_size = data.max_buffer_size;
                    }
                    return *this;
                }
        };

        class State {
            public:
                bool on = false;
                bool off = true;
                bool trigger = false;
                State& operator=(const State &data) {
                    if (this != &data) {
                        on = data.on;
                        off = data.off;
                        trigger = data.trigger;
                    }
                    return *this;
                }
        };

    public:
        Trigger_Source trigger_source;
        Data data;
        State state;
        void open(Device::Data device_data, double sampling_frequency = 20e06, int buffer_size = 0, double offset = 0, double amplitude_range = 5);
        double measure(Device::Data device_data, int channel);
        void trigger(Device::Data device_data, bool enable, const TRIGSRC source = trigsrcNone, int channel = 1, double timeout = 0, bool edge_rising = true, double level = 0);
        std::vector<double> record(Device::Data device_data, int channel);
        void close(Device::Data device_data);
} scope;

}
#endif
