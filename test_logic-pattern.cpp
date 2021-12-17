#include "WF_SDK/WF_SDK.h"  // include all classes and functions

#include <iostream>         // needed for input/output
#include <fstream>

using namespace std;

/* ----------------------------------------------------- */

int main(void) {
    // connect to the device
    device_data this_device;
    this_device = device.open();

    // check for connection errors
    device.check_error(this_device.handle);

    /* ----------------------------------------------------- */

    // use instruments here
    // initialize the logic analyzer with default settings
    logic.open(this_device.handle);

    // set up triggering on DIO channel 0
    logic.trigger(this_device.handle, true, 0, 4096, 0, 0, false);

    // generate a 100KHz PWM signal with 30% duty cycle on DIO0
    pattern.generate(this_device.handle, 0, pattern.function.pulse, 100e03, 30);

    // record data on DIO 0
    logic_data recorded_data;
    recorded_data = logic.record(this_device.handle, 0);

    // convert the time base
    for_each(recorded_data.time.begin(), recorded_data.time.end(), [](double &element){ element *= 1e06; });

    // save data
    ofstream file;
    file.open("test_logic-pattern.csv");
    file << "time [us],logic value\n";
    for (int index = 0; index < recorded_data.buffer.size(); index++) {
        file << to_string(recorded_data.time[index]) + "," + to_string(recorded_data.buffer[index]) + "\n";
    }
    file.close();

    // plot
    system("python plotting.py test_logic-pattern.csv");

    // reset the logic analyzer
    logic.close(this_device.handle);

    // reset the pattern generator
    pattern.close(this_device.handle);

    /* ----------------------------------------------------- */

    // close the connection
    device.close(this_device.handle);

    cout << "\nPress Enter to exit...";
    cin.get();
    return 0;
}
