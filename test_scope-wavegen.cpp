#include "WF_SDK/WF_SDK.h"  // include all classes and functions

#include "matplotlib/matplotlibcpp.h"   // needed for plotting (https://github.com/lava/matplotlib-cpp)
#include <vector>
namespace plt = matplotlibcpp;

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

        // convert data types
        std::vector<double> xdata;
        std::vector<double> ydata;
        for (int index = 0; index < sizeof(recorded_data.buffer) / sizeof(recorded_data.buffer[0]); index++) {
            xdata[index] = recorded_data.time[index] * 1e03;
            ydata[index] = recorded_data.buffer[index];
        }

        // plot
        plt::plot(xdata, ydata);
        plt::xlabel("time [ms]");
        plt::ylabel("voltage [V]");
        plt::show();

        // reset the scope
        scope.close(this_device.handle);

        // reset the wavegen
        wavegen.close(this_device.handle);
    }

    /* ----------------------------------------------------- */

    // close the connection
    device.close(this_device.handle);
    return;
}
