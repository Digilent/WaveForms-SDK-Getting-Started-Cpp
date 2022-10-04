/* STATIC I/O CONTROL FUNCTIONS: set_mode, get_state, set_state, set_current, set_pull, close */

/* include the necessary libraries */
#include <algorithm>
#include <math.h>
#include <string>
#include "dwf.h"
#include "device.h"
#include "tools.h"

#ifndef WF_STATIC
#define WF_STATIC
namespace wf {

class Static {
    private:
        unsigned int rotate_left(unsigned int number, unsigned int position, unsigned int size = 16);

        class Data {
            public:
                int channel = -1;
                int count = 0;
                class Nodes {
                    public:
                        int current = -1;
                        int pull_enable = -1;
                        int pull_direction = -1;
                        int pull_weak = -1;
                        Nodes& operator=(const Nodes &data) {
                            if (this != &data) {
                                current = data.current;
                                pull_enable = data.pull_enable;
                                pull_direction = data.pull_direction;
                                pull_weak = data.pull_weak;
                            }
                            return *this;
                        }
                } nodes;
                Data& operator=(const Data &data) {
                    if (this != &data) {
                        channel = data.channel;
                        count = data.count;
                        nodes = data.nodes;
                    }
                    return *this;
                }
        };

        class Pull {
            public:
                const double down = 0;
                const double idle = 0.5;
                const double up = 1;
        };

    public:
        Data data;
        Pull pull;
        void set_mode(Device::Data *device_data, int channel, bool output);
        bool get_state(Device::Data *device_data, int channel);
        void set_state(Device::Data *device_data, int channel, bool value);
        void set_current(Device::Data *device_data, double current);
        void set_pull(Device::Data *device_data, int channel, double dirtection);
        void close(Device::Data *device_data);
} static_;

}
#endif
