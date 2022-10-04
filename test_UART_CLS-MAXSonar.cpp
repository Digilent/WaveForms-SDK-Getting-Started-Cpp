#include "WF_SDK/WF_SDK.h"  // include all classes and functions
#include <iostream>         // needed for input/output
#include <sstream>          // needed for string manipulation
#include <string>           // needed for error handling

using namespace wf;

int reset = 2;      // define MAXSonar reset line
int timeout = 1000; // define timeout iteration count

/* ----------------------------------------------------- */

int main(void) {
    // connect to the device
    Device::Data *device_data;
    try {
        device_data = device.open();

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

        // initialize the reset line
        static_.set_mode(device_data, reset, true);
        static_.set_state(device_data, reset, false);

        // initialize the uart interface on DIO0 and DIO1
        uart.open(device_data, 1, 0, 9600);

        tools.keyboard_interrupt_reset(device_data);

        std::cout << "Press Ctrl+C to exit..." << std::endl;

        while (true) {
            try {
                // repeat
                // clear the screen and home cursor
                uart.write(device_data, "\x1b[j");

                // display a message
                uart.write(device_data, "Dist: ");

                // read raw data
                static_.set_state(device_data, reset, true);    // enable the device
                int current_timeout = timeout;
                std::vector<unsigned char> rx_data;
                while (current_timeout > 0) {
                    // wait for data
                    rx_data = uart.read(device_data);
                    if (rx_data.size() > 0) {
                        // exit when data is received
                        break;
                    }
                    current_timeout--;
                }
                static_.set_state(device_data, reset, false);    // disable the device

                // convert raw data into distance
                int value = 0;
                if (rx_data[0] == 234) {
                    for (int index = 1; index < rx_data.size(); index++) {
                        if (rx_data[index] > 47 && rx_data[index] < 58) {
                            // concatenate valid bytes
                            value = value * 10 + (rx_data[index] - 48);
                        }
                    }
                    value *= 2.54;   // convert to cm
                }
                else {
                    // error in message
                    value = -1;
                }

                // display the distance
                std::ostringstream output;
                output.precision(2);
                output << std::fixed << value;
                uart.write(device_data, output.str());

                // display a message
                uart.write(device_data, "cm");

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
    return 0;
}
