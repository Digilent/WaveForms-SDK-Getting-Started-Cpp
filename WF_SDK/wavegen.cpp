/* WAVEFORM GENERATOR CONTROL FUNCTIONS: generate, close */

/* include the header */
#include "wavegen.h"

/* ----------------------------------------------------- */

void Wavegen::generate(Device::Data device_data, int channel, FUNC function, double offset, double frequency, double amplitude, double symmetry, double wait, double run_time, int repeat, std::vector<double> data) {
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
    FDwfAnalogOutNodeEnableSet(device_data.handle, channel, AnalogOutNodeCarrier, true);
    
    // set function type
    FDwfAnalogOutNodeFunctionSet(device_data.handle, channel, AnalogOutNodeCarrier, function);
    
    // load data if the function type is custom
    if (function == funcCustom) {
        FDwfAnalogOutNodeDataSet(device_data.handle, channel, AnalogOutNodeCarrier, data.data(), data.size());
    }
    
    // set frequency
    FDwfAnalogOutNodeFrequencySet(device_data.handle, channel, AnalogOutNodeCarrier, frequency);
    
    // set amplitude or DC voltage
    FDwfAnalogOutNodeAmplitudeSet(device_data.handle, channel, AnalogOutNodeCarrier, amplitude);
    
    // set offset
    FDwfAnalogOutNodeOffsetSet(device_data.handle, channel, AnalogOutNodeCarrier, offset);
    
    // set symmetry
    FDwfAnalogOutNodeSymmetrySet(device_data.handle, channel, AnalogOutNodeCarrier, symmetry);
    
    // set running time limit
    FDwfAnalogOutRunSet(device_data.handle, channel, run_time);
    
    // set wait time before start
    FDwfAnalogOutWaitSet(device_data.handle, channel, wait);
    
    // set number of repeating cycles
    FDwfAnalogOutRepeatSet(device_data.handle, channel, repeat);
    
    // start
    FDwfAnalogOutConfigure(device_data.handle, channel, true);
    state.on = true;
    state.off = false;
    state.channel[channel] = true;
    return;
}

/* ----------------------------------------------------- */

void Wavegen::close(Device::Data device_data, int channel) {
    /*
        reset the wavegen
    */
    channel--;
    FDwfAnalogOutReset(device_data.handle, channel);
    state.on = false;
    state.off = true;
    if (channel >= 0) {
        state.channel[channel] = false;
    }
    else {
        state.channel.clear();
        state.channel.insert(state.channel.end(), false);
        state.channel.insert(state.channel.end(), false);
    }
    return;
}

/* ----------------------------------------------------- */

void Wavegen::enable(Device::Data device_data, int channel) {
    /*
        enables an analog output channel
    */
    channel--;
    FDwfAnalogOutConfigure(device_data.handle, channel, true);
    state.on = true;
    state.off = false;
    state.channel[channel] = true;
    return;
}

/* ----------------------------------------------------- */

void Wavegen::disable(Device::Data device_data, int channel) {
    /*
        disables an analog output channel
    */
    channel--;
    FDwfAnalogOutConfigure(device_data.handle, channel, false);
    state.channel[channel] = false;
    return;
}
