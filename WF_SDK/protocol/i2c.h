/* PROTOCOL: I2C CONTROL FUNCTIONS: open, read, write, exchange, close */

/* include the necessary libraries */
#include <string>
#include <vector>
#include <cstdio>
#include "dwf.h"
#include "../device.h"

#ifndef WF_PROTOCOL_I2C
#define WF_PROTOCOL_I2C
namespace wf {

class I2C {
    private:
        void check_warning(Device::Data *device_data, int nak, const char *caller = __builtin_FUNCTION());
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

    public:
        void open(Device::Data *device_data, int sda, int scl, double clk_rate = 100e03, bool stretching = true);
        std::vector<unsigned char> read(Device::Data *device_data, int count, int address);
        void write(Device::Data *device_data, std::string data, int address);
        void write(Device::Data *device_data, std::vector<unsigned char> data, int address);
        std::vector<unsigned char> exchange(Device::Data *device_data, std::string tx_data, int count, int address);
        std::vector<unsigned char> exchange(Device::Data *device_data, std::vector<unsigned char> tx_data, int count, int address);
        //std::vector<unsigned char> spy(Device::Data device_data, int count = 16);
        void close(Device::Data *device_data);
} i2c;

}
#endif
