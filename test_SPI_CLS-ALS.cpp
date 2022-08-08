#include "WF_SDK/WF_SDK.h"  // include all classes and functions
#include <iostream>         // needed for input/output
#include <vector>           // needed for vector handling
#include <sstream>          // needed for string manipulation

// define DIO lines
int CLS_cs = 0;
int ALS_cs = 1;
int sck = 2;
int miso = 3;
int mosi = 4;

/* ----------------------------------------------------- */

int main(void) {
    // connect to the device
    Device::Data device_data = device.open();

    // check for connection errors
    device.check_error(device_data);

    /* ----------------------------------------------------- */

    // start the positive supply
    Supplies::Data supplies_data;
    supplies_data.master_state = true;
    supplies_data.positive_state = true;
    supplies_data.positive_voltage = 3.3;
    supplies.switch_(device_data, supplies_data);
    tools.sleep(500);    // delay

    // initialize the spi interface on DIO0, DIO1, DIO2, DIO3 and DIO4
    spi.open(device_data, CLS_cs, sck, miso, mosi);
    spi.open(device_data, ALS_cs, sck, miso, mosi);

    tools.keyboard_interrupt_reset(device_data);

    std::cout << "Press Ctrl+C to exit..." << std::endl;

    while (true) {
        // repeat
        // clear the screen and home cursor
        spi.write(device_data, "\x1b[j", CLS_cs);

        // display a message
        spi.write(device_data, "Lum: ", CLS_cs);

        // read the temperature
        std::vector<unsigned char> message = spi.read(device_data, 2, ALS_cs);
        double value = ((int(message[0]) << 3) | (int(message[1]) >> 4)) / 1.27;

        // display the temperature
        std::ostringstream output;
        output.precision(2);
        output << std::fixed << value;
        spi.write(device_data, output.str(), CLS_cs);

        // display a message
        spi.write(device_data, "%", CLS_cs);

        // delay 1s
        tools.sleep(1000);
    }

    return 0;
}
