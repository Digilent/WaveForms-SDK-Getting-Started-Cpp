#include "WF_SDK/WF_SDK.h"  // include all classes and functions

#include <iostream>         // needed for input/output
#include <fstream>
#include <vector>

/* ----------------------------------------------------- */

int main(void) {
    // connect to the device
    Device::Data device_data = device.open();

    // check for connection errors
    device.check_error(device_data);

    /* ----------------------------------------------------- */

    // use instruments here
    if (device_data.name != "Digital Discovery") {
        // initialize the scope with default settings
        scope.open(device_data);

        // set up triggering on scope channel 1
        scope.trigger(device_data, true, scope.trigger_source.analog, 1, 0);

        // generate a 10KHz sine signal with 2V amplitude on channel 1
        wavegen.generate(device_data, 1, wavegen.function.sine, 0, 10e03, 2);

        // record data with the scope on channel 1
        std::vector<double> buffer = scope.record(device_data, 1);

        // limit displayed data size
        int length = buffer.size();
        if (length > 10000) {
            length = 10000;
        }
        
        // save data
        std::ofstream file;
        file.open("test_scope-wavegen.csv");
        file << "time [ms],voltage [V]\n";
        for (int index = 0; index < length; index++) {
            file << std::to_string(index * 1e03 / scope.data.sampling_frequency) << "," << std::to_string(buffer[index]) << std::endl;
        }
        file.close();

        // plot
        system("python plotting.py test_scope-wavegen.csv");

        // reset the scope
        scope.close(device_data);

        // reset the wavegen
        wavegen.close(device_data);
    }

    /* ----------------------------------------------------- */

    // close the connection
    device.close(device_data);
    return 0;
}
