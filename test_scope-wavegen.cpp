#include "WF_SDK/WF_SDK.h"  // include all classes and functions
#include <iostream>         // needed for input/output
#include <string>           // needed for error handling
#include <fstream>
#include <vector>

using namespace wf;

/* ----------------------------------------------------- */

int main(void) {
    // connect to the device
    Device::Data *device_data;
    try {
        device_data = device.open();

        /* ----------------------------------------------------- */

        // use instruments here
        if (device_data->name != "Digital Discovery") {
            // initialize the scope with default settings
            scope.open(device_data);

            // set up triggering on scope channel 1
            scope.trigger(device_data, true, scope.trigger_source.analog, 1, 0);

            // generate a 10KHz sine signal with 2V amplitude on channel 1
            wavegen.generate(device_data, 1, wavegen.function.sine, 0, 10e03, 2);
            tools.sleep(1000);

            // record data with the scope on channel 1
            std::vector<double> buffer = scope.record(device_data, 1);

            // limit displayed data size
            int length = buffer.size();
            if (length > 10000) {
                length = 10000;
            }
            buffer.resize(length);
            
            // save time domain data
            std::ofstream file;
            file.open("test_scope-wavegen.csv");
            file << "time [ms],voltage [V]\n";
            for (int index = 0; index < buffer.size(); index++) {
                file << std::to_string(index * 1e03 / scope.data.sampling_frequency) << "," << std::to_string(buffer[index]) << std::endl;
            }
            file.close();

            // plot
            system("python plotting.py test_scope-wavegen.csv");

            // compute the spectrum from 0Hz to 100KHz
            double start_frequency = 0;
            double stop_frequency = 100e03;
            std::vector<double> spectrum = tools.spectrum(buffer, tools.window.flat_top, scope.data.sampling_frequency, start_frequency, stop_frequency);

            // save frequency domain data
            file.open("test_scope-wavegen_spectrum.csv");
            file << "frequency [MHz],magnitude [dBV]\n";
            double step = (stop_frequency - start_frequency) / (spectrum.size() - 1);
            for (int index = 0; index < spectrum.size(); index++) {
                file << std::to_string((start_frequency + index * step) / 1e06) << "," << std::to_string(spectrum[index]) << std::endl;
            }
            file.close();

            // plot
            system("python plotting.py test_scope-wavegen_spectrum.csv");

            // reset the scope
            scope.close(device_data);

            // reset the wavegen
            wavegen.close(device_data);
        }

        /* ----------------------------------------------------- */

        // close the connection
        device.close(device_data);
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
