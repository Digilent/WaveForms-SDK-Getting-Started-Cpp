#include "WF_SDK/WF_SDK.h"  // include all classes and functions

#include <iostream>         // needed for input/output
#include <fstream>

using namespace std;

/* ----------------------------------------------------- */

void main(void) {
    // connect to the device
    device_data this_device;
    this_device = device.open();

    // check for connection errors
    device.check_error(this_device.handle);

    /* ----------------------------------------------------- */

    // use instruments here
    if (this_device.name != "Digital Discovery") {
        // initialize the scope with default settings
        scope.open(this_device.handle);

        // set up triggering on scope channel 1
        scope.trigger(this_device.handle, true, scope.trigger_source.analog, 1, 0);

        // generate a 10KHz sine signal with 2V amplitude on channel 1
        wavegen.generate(this_device.handle, 1, wavegen.function.sine, 0, 10e03, 2);

        // record data with the scopeon channel 1
        scope_data recorded_data;
        recorded_data = scope.record(this_device.handle, 1);

        // convert the time base
        for_each(recorded_data.time.begin(), recorded_data.time.end(), [](double &element){ element *= 1e03; });

        // save data
        ofstream file;
        file.open("test_scope-wavegen.csv");
        file << "time [ms],voltage [V]\n";
        for (int index = 0; index < recorded_data.buffer.size(); index++) {
            file << to_string(recorded_data.time[index]) + "," + to_string(recorded_data.buffer[index]) + "\n";
        }
        file.close();

        // plot
        system("python plotting.py test_scope-wavegen.csv");

        // reset the scope
        scope.close(this_device.handle);

        // reset the wavegen
        wavegen.close(this_device.handle);
    }

    /* ----------------------------------------------------- */

    // close the connection
    device.close(this_device.handle);

    cout << "\nPress Enter to exit...";
    cin.get();
    return;
}
