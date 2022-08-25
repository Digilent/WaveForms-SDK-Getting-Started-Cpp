/* PROTOCOL: SPI CONTROL FUNCTIONS: open, read, write, exchange, close */

/* include the necessary libraries */
#include <string>
#include <vector>
#include "dwf.h"
#include "../device.h"

#ifndef WF_PROTOCOL_SPI
#define WF_PROTOCOL_SPI
namespace wf {

class SPI {
    private:
        class State {
            public:
                bool on = false;
                bool off = true;
                double frequency = 0;
                int mode = -1;
                bool order = bool(-1);
                State& operator=(const State &data) {
                    if (this != &data) {
                        on = data.on;
                        off = data.off;
                        frequency = data.frequency;
                        mode = data.mode;
                        order = data.order;
                    }
                    return *this;
                }

        };
        std::vector<unsigned char> convert(unsigned long long number);
    public:
        State state;
        void open(Device::Data device_data, int cs, int sck, int miso = -1, int mosi = -1, double clk_frequency = 1e06, int mode = 0, bool order = true);
        std::vector<unsigned char> read(Device::Data device_data, int count, int cs);
        void write(Device::Data device_data, std::string data, int cs);
        void write(Device::Data device_data, std::vector<unsigned char> data, int cs);
        std::vector<unsigned char> exchange(Device::Data device_data, std::string tx_data, int count, int cs);
        std::vector<unsigned char> exchange(Device::Data device_data, std::vector<unsigned char> tx_data, int count, int cs);
        //spi_data spy(Device::Data device_data, int count, int cs, int sck, int mosi = -1, int miso = -1, int word_size = 8);
        void close(Device::Data device_data);
} spi;

}
#endif
