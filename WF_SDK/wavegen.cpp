/* WAVEFORM GENERATOR CONTROL FUNCTIONS: generate, close */

/* include the header */
#include "wavegen.h"

/* ----------------------------------------------------- */

void wf::Wavegen::generate(Device::Data *device_data, int channel, FUNC function, double offset, double frequency, double amplitude, double symmetry, double wait, double run_time, int repeat, std::vector<double> data) {
    /*
        generate an analog signal

        parameters: - device data
                    - the selected wavegen channel (1-2)
                    - function - possible: custom, sine, square, triangle, noise, ds, pulse, trapezium, sine_power, ramp_up, ramp_down
                    - offset voltage in Volts
                    - frequency in Hz, default is 1KHz
                    - amplitude in Volts, default is 1V
                    - signal symmetry in percentage, default is 50%
                    - wait time in seconds, default is 0s
                    - run time in seconds, default is infinite (0)
                    - repeat count, default is infinite (0)
                    - data - list of voltages, used only if function=custom, default is empty
    */
    // enable channel
    channel--;
    if (FDwfAnalogOutNodeEnableSet(device_data->handle, channel, AnalogOutNodeCarrier, true) == 0) {
        device.check_error(device_data);
    }
    
    // set function type
    if (FDwfAnalogOutNodeFunctionSet(device_data->handle, channel, AnalogOutNodeCarrier, function) == 0) {
        device.check_error(device_data);
    }
    
    // load data if the function type is custom
    if (function == funcCustom) {
        if (FDwfAnalogOutNodeDataSet(device_data->handle, channel, AnalogOutNodeCarrier, data.data(), data.size()) == 0) {
            device.check_error(device_data);
        }
    }
    
    // set frequency
    if (FDwfAnalogOutNodeFrequencySet(device_data->handle, channel, AnalogOutNodeCarrier, frequency) == 0) {
        device.check_error(device_data);
    }
    
    // set amplitude or DC voltage
    if (FDwfAnalogOutNodeAmplitudeSet(device_data->handle, channel, AnalogOutNodeCarrier, amplitude) == 0) {
        device.check_error(device_data);
    }
    
    // set offset
    if (FDwfAnalogOutNodeOffsetSet(device_data->handle, channel, AnalogOutNodeCarrier, offset) == 0) {
        device.check_error(device_data);
    }
    
    // set symmetry
    if (FDwfAnalogOutNodeSymmetrySet(device_data->handle, channel, AnalogOutNodeCarrier, symmetry) == 0) {
        device.check_error(device_data);
    }
    
    // set running time limit
    if (FDwfAnalogOutRunSet(device_data->handle, channel, run_time) == 0) {
        device.check_error(device_data);
    }
    
    // set wait time before start
    if (FDwfAnalogOutWaitSet(device_data->handle, channel, wait) == 0) {
        device.check_error(device_data);
    }
    
    // set number of repeating cycles
    if (FDwfAnalogOutRepeatSet(device_data->handle, channel, repeat) == 0) {
        device.check_error(device_data);
    }
    
    // start
    if (FDwfAnalogOutConfigure(device_data->handle, channel, true) == 0) {
        device.check_error(device_data);
    }
    return;
}

/* ----------------------------------------------------- */

void wf::Wavegen::close(Device::Data *device_data, int channel) {
    /*
        reset the wavegen
    */
    channel--;
    if (FDwfAnalogOutReset(device_data->handle, channel) == 0) {
        device.check_error(device_data);
    }
    return;
}

/* ----------------------------------------------------- */

void wf::Wavegen::enable(Device::Data *device_data, int channel) {
    /*
        enables an analog output channel
    */
    channel--;
    if (FDwfAnalogOutConfigure(device_data->handle, channel, true) == 0) {
        device.check_error(device_data);
    }
    return;
}

/* ----------------------------------------------------- */

void wf::Wavegen::disable(Device::Data *device_data, int channel) {
    /*
        disables an analog output channel
    */
    channel--;
    if (FDwfAnalogOutConfigure(device_data->handle, channel, false) == 0) {
        device.check_error(device_data);
    }
    return;
}
