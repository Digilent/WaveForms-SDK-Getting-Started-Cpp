#include "WF_SDK/WF_SDK.h"  // include all classes and functions
#include <iostream>         // needed for input/output
#include <vector>           // needed for vector handling
#include <sstream>          // needed for string manipulation
#include <string>           // needed for error handling

using namespace wf;

// define DIO lines
int sda = 0;
int scl = 1;

// define addresses
int CLS_address = 0x48;
int TMP2_address = 0x4B;

/* ----------------------------------------------------- */

int main(void) {
    // connect to the device
    Device::Data *device_data;
    try {
        device_data = device.open();

        /* ----------------------------------------------------- */

        // start the positive supply
        Supplies::Data supplies_data;
        supplies_data.master_state = true;
        supplies_data.positive_state = true;
        supplies_data.positive_voltage = 3.3;
        supplies.switch_(device_data, supplies_data);
        tools.sleep(500);    // delay

        // initialize the i2c interface on DIO0 and DIO1
        i2c.open(device_data, sda, scl);

        // initialize the PMOD TMP2 (set output size to 16-bit)
        std::vector<unsigned char> input_data {0x03, 0x80};
        i2c.write(device_data, input_data, TMP2_address);

        // save custom character
        i2c.write(device_data, "\x1b[7;5;7;0;0;0;0;0;0d", CLS_address);   // define character
        i2c.write(device_data, "\x1b[3p", CLS_address); // load character table

        tools.keyboard_interrupt_reset(device_data);

        std::cout << "Press Ctrl+C to exit..." << std::endl;

        while (true) {
            try {
                // repeat
                // clear the screen and home cursor
                std::vector<unsigned char> input_data1 {0x1B, 0x5B, 0x6A};
                i2c.write(device_data, input_data1, CLS_address);

                // display a message
                i2c.write(device_data, "Temp: ", CLS_address);

                // read the temperature
                std::vector<unsigned char> output_data = i2c.read(device_data, 2, TMP2_address);   // read 2 bytes
                int value = (int(output_data[0]) << 8) | int(output_data[1]);    // create integer from received bytes
                double result;
                if ((value >> 15) & 1 == 0) {
                    result = value / 128.0;    // decode positive numbers
                }
                else {
                    result = (value - 65535) / 128.0;   // decode negative numbers
                }

                // display the temperature
                std::ostringstream output;
                output.precision(2);
                output << std::fixed << result;
                i2c.write(device_data, output.str(), CLS_address);

                // display a message
                std::vector<unsigned char> input_data2 {0};
                i2c.write(device_data, input_data2, CLS_address);
                i2c.write(device_data, "C", CLS_address);

                // delay 1s
                tools.sleep(1000);
            }
            catch (Warning warning) {
                // if a warning occurs display it
                std::cout << "Warning: ";
                std::cout << warning.instrument << " -> ";
                std::cout << warning.function << " -> ";
                std::cout << warning.message << std::endl;
            }
        }
    }

    catch (Error error) {
        // if an error occurs display it
        std::cout << "Error: ";
        std::cout << error.instrument << " -> ";
        std::cout << error.function << " -> ";
        std::cout << error.message << std::endl;
        // close the connection
        device.close(device_data);
    }
    catch (Warning warning) {
        // if a warning occurs display it
        std::cout << "Warning: ";
        std::cout << warning.instrument << " -> ";
        std::cout << warning.function << " -> ";
        std::cout << warning.message << std::endl;
        // close the connection
        device.close(device_data);
    }
    return 0;
}
