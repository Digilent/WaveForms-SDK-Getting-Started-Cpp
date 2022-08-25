#include "WF_SDK/WF_SDK.h"  // include all classes and functions
#include <iostream>         // needed for input/output
#include <fstream>
#include <vector>

using namespace wf;

#define DIO_IN 14
#define DIO_OUT 14

/* ----------------------------------------------------- */

int main(void) {
    // connect to the device
    Device::Data device_data = device.open();

    // check for connection errors
    device.check_error(device_data);

    /* ----------------------------------------------------- */

    // use instruments here
    // initialize the logic analyzer with default settings
    logic.open(device_data);

    // set up triggering on selected DIO
    logic.trigger(device_data, true, DIO_IN, 0, 0, false);

    // generate a 100KHz PWM signal with 30% duty cycle on selected DIO
    pattern.generate(device_data, DIO_OUT, pattern.function.pulse, 100e03, 30);

    // wait 1 second
    tools.sleep(1000);

    // record data on selected DIO
    std::vector<unsigned short> buffer = logic.record(device_data, DIO_IN);

    // limit displayed data size
    int length = buffer.size();
    if (length > 10000) {
        length = 10000;
    }

    // save data
    std::ofstream file;
    file.open("test_logic-pattern.csv");
    file << "time [us],logic value\n";
    for (int index = 0; index < length; index++) {
        file << std::to_string(index * 1e06 / logic.data.sampling_frequency) << "," << std::to_string(buffer[index]) << std::endl;
    }
    file.close();

    // plot
    system("python plotting.py test_logic-pattern.csv");

    // reset the logic analyzer
    logic.close(device_data);

    // reset the pattern generator
    pattern.close(device_data);

    /* ----------------------------------------------------- */

    // close the connection
    device.close(device_data);
    return 0;
}
