/* STATIC I/O CONTROL FUNCTIONS: set_mode, get_state, set_state, set_current, set_pull, close */

/* include the necessary libraries */
#include <algorithm>
#include <math.h>
#include "dwf.h"
#include "device.h"

#ifndef WF_STATIC
#define WF_STATIC
namespace wf {

class Static {
    private:
        unsigned int rotate_left(unsigned int number, unsigned int position, unsigned int size = 16);
        class State {
            public:
                bool state[16];
                bool input[16];
                bool output[16];
                bool pull[16];
                int current = 0;
                State& operator=(const State &data) {
                    if (this != &data) {
                        current = data.current;
                        for (int index = 0; index < 16; index++) {
                            state[index] = data.state[index];
                            input[index] = data.input[index];
                            output[index] = data.output[index];
                            pull[index] = data.pull[index];
                        }
                    }
                    return *this;
                }
        };
    public:
        State state;
        void set_mode(Device::Data device_data, int channel, bool output);
        bool get_state(Device::Data device_data, int channel);
        void set_state(Device::Data device_data, int channel, bool value);
        void set_current(Device::Data device_data, int current);
        void set_pull(Device::Data device_data, int channel, bool direction = bool(-1));
        void close(Device::Data device_data);
} static_;

}
#endif
