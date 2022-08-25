/* PROTOCOL: I2C CONTROL FUNCTIONS: open, read, write, exchange, close */

/* include the necessary libraries */
#include <string>
#include <vector>
#include "dwf.h"
#include "../device.h"

#ifndef WF_PROTOCOL_I2C
#define WF_PROTOCOL_I2C
namespace wf {

class I2C {
    private:
        /*class Data {
            public:
                std::vector<unsigned char> data;
                std::string error;
                int address;
                std::string start;
                std::string stop;
                std::string direction;
                Data& operator=(const Data &data) {
                    if (this != &data) {
                        error = data.error;
                        address = data.address;
                        start = data.start;
                        stop = data.stop;
                        direction = data.direction;
                    }
                    return *this;
                }
        };*/

        class State {
            public:
                bool on = false;
                bool off = true;
                double frequency = 0;
                State& operator=(const State &data) {
                    if (this != &data) {
                        on = data.on;
                        off = data.off;
                        frequency = data.frequency;
                    }
                    return *this;
                }
        };

    public:
        State state;
        void open(Device::Data device_data, int sda, int scl, double clk_rate = 100e03, bool stretching = true, std::string *error=NULL);
        std::vector<unsigned char> read(Device::Data device_data, int count, int address, std::string *error=NULL);
        void write(Device::Data device_data, std::string data, int address, std::string *error=NULL);
        void write(Device::Data device_data, std::vector<unsigned char> data, int address, std::string *error=NULL);
        std::vector<unsigned char> exchange(Device::Data device_data, std::string tx_data, int count, int address, std::string *error=NULL);
        std::vector<unsigned char> exchange(Device::Data device_data, std::vector<unsigned char> tx_data, int count, int address, std::string *error=NULL);
        //std::vector<unsigned char> spy(Device::Data device_data, int count = 16);
        void close(Device::Data device_data);
} i2c;

}
#endif
