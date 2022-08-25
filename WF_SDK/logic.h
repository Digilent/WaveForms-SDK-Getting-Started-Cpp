/* LOGIC ANALYZER CONTROL FUNCTIONS: open, trigger, record, close */

/* include the necessary libraries */
#include <vector>
#include "dwf.h"
#include "device.h"

#ifndef WF_LOGIC
#define WF_LOGIC
namespace wf {

class Logic {
    private:
        class Data {
            public:
                int sampling_frequency = 100e06;
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

        int min(int a, int b);
        int max(int a, int b);

    public:
        Data data;
        State state;
        void open(Device::Data device_data, double sampling_frequency = 100e06, int buffer_size = 0);
        void trigger(Device::Data device_data, bool enable, int channel, int position = 0, double timeout = 0, bool rising_edge = true, double length_min = 0, double length_max = 20, int count = 0);
        std::vector<unsigned short> record(Device::Data device_data, int channel);
        void close(Device::Data device_data);
} logic;

}
#endif
