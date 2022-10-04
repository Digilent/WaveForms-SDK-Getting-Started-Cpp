/* PROTOCOL: UART CONTROL FUNCTIONS: open, read, write, close */

/* include the necessary libraries */
#include <string>
#include <vector>
#include "dwf.h"
#include "../device.h"

#ifndef WF_PROTOCOL_UART
#define WF_PROTOCOL_UART
namespace wf {

class UART {
    public:
        void open(Device::Data *device_data, int rx, int tx, int baud_rate = 9600, bool parity = bool(-1), int data_bits = 8, int stop_bits = 1);
        std::vector<unsigned char> read(Device::Data *device_data);
        void write(Device::Data *device_data, std::string data);
        void write(Device::Data *device_data, std::vector<unsigned char> data);
        void close(Device::Data *device_data);
} uart;

}
#endif
