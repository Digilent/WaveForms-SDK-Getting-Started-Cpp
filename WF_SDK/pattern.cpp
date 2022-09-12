/* PATTERN GENERATOR CONTROL FUNCTIONS: generate, close */

/* include the header */
#include "pattern.h"

/* ----------------------------------------------------- */

void wf::Pattern::generate(Device::Data device_data, int channel, DwfDigitalOutType function, double frequency, double duty_cycle, std::vector<unsigned short> data, double wait, int repeat, int run_time, DwfDigitalOutIdle idle, bool trigger_enabled, TRIGSRC trigger_source, bool trigger_edge_rising) {
    /*
        generate a logic signal
        
        parameters: - channel - the selected DIO line number
                    - function - possible: pulse, custom, random
                    - frequency in Hz
                    - duty cycle in percentage, used only if function = pulse, default is 50%
                    - data list, used only if function = custom, default is empty
                    - wait time in seconds, default is 0 seconds
                    - repeat count, default is infinite (0)
                    - run time in seconds, 0=infinite, -1=auto
                    - idle state
                    - trigger_enabled - include/exclude trigger from repeat cycle
                    - trigger_source - possible: none, analog, digital, external[1-4]
                    - trigger_edge_rising - True means rising, False means falling, None means either, default is rising
    */
    if (device_data.name == std::string("Digital Discovery")) {
        channel = channel - 24;
    }
    
    // get internal clock frequency
    double internal_frequency = 0;
    FDwfDigitalOutInternalClockInfo(device_data.handle, &internal_frequency);
    
    // get counter value range
    unsigned int counter_limit = 0;
    FDwfDigitalOutCounterInfo(device_data.handle, 0, 0, &counter_limit);
    
    // calculate the divider for the given signal frequency
    int divider;
    if (function == DwfDigitalOutTypePulse) {
        divider = int(ceil(internal_frequency / frequency / counter_limit));
    }
    else {
        divider = int(internal_frequency / frequency);
    }
    
    // enable the respective channel
    FDwfDigitalOutEnableSet(device_data.handle, channel, 1);
    
    // set output type
    FDwfDigitalOutTypeSet(device_data.handle, channel, function);
    
    // set frequency
    FDwfDigitalOutDividerSet(device_data.handle, channel, divider);

    // set idle state
    FDwfDigitalOutIdleSet(device_data.handle, channel, idle);

    // calculate run length
    if (run_time < 0) {
        run_time = data.size() / frequency;
    }
    FDwfDigitalOutRunSet(device_data.handle, run_time);
    
    // set wait time
    FDwfDigitalOutWaitSet(device_data.handle, wait);
    
    // set repeat count
    FDwfDigitalOutRepeatSet(device_data.handle, repeat);
    
    // enable triggering
    FDwfDigitalOutRepeatTriggerSet(device_data.handle, int(trigger_enabled));
    
    if (trigger_enabled == true) {
        // set trigger source
        FDwfDigitalOutTriggerSourceSet(device_data.handle, trigger_source);
    
        // set trigger slope
        if (trigger_edge_rising == true) {
            // rising edge
            FDwfDigitalOutTriggerSlopeSet(device_data.handle, DwfTriggerSlopeRise);
        }
        else if (trigger_edge_rising == false) {
            // falling edge
            FDwfDigitalOutTriggerSlopeSet(device_data.handle, DwfTriggerSlopeFall);
        }
        else {
            // either edge
            FDwfDigitalOutTriggerSlopeSet(device_data.handle, DwfTriggerSlopeEither);
        }
    }

    // set PWM signal duty cycle
    if (function == DwfDigitalOutTypePulse) {
        // calculate counter steps to get the required frequency
        int steps = int(round(internal_frequency / frequency / divider));
        // calculate steps for low and high parts of the period
        int high_steps = int(steps * duty_cycle / 100);
        int low_steps = steps - high_steps;
        FDwfDigitalOutCounterSet(device_data.handle, channel, low_steps, high_steps);
    }
    
    // load custom signal data
    else if (function == DwfDigitalOutTypeCustom) {
        // format data
        int buffer_length = (data.size() + 7) >> 3;
        std::vector<unsigned short> buffer(buffer_length);
        for (int index = 0; index < data.size(); index++) {
            buffer[index] = 0;
            if (data[index] != 0) {
                buffer[index >> 3] |= 1 << (index & 7);
            }
        }
    
        // load data
        FDwfDigitalOutDataSet(device_data.handle, channel, buffer.data(), buffer.size());
    }
    
    // start generating the signal
    FDwfDigitalOutConfigure(device_data.handle, true);
    state.on = true;
    state.off = false;
    state.channel[channel] = true;
    return;
}

/* ----------------------------------------------------- */

void wf::Pattern::close(Device::Data device_data) {
    /*
        reset the instrument
    */
    FDwfDigitalOutReset(device_data.handle);
    state.on = false;
    state.off = true;
    for (int index = 0; index < state.channel.size(); index++) {
        state.channel[index] = false;
    }
    return;
}

/* ----------------------------------------------------- */

void wf::Pattern::enable(Device::Data device_data, int channel) {
    /* enables a digital output channel */
    if (device_data.name == std::string("Digital Discovery")) {
        channel = channel - 24;
    }
    FDwfDigitalOutEnableSet(device_data.handle, channel, 1);
    FDwfDigitalOutConfigure(device_data.handle, true);
    state.on = true;
    state.off = false;
    state.channel[channel] = true;
    return;
}

/* ----------------------------------------------------- */

void wf::Pattern::disable(Device::Data device_data, int channel) {
    /* disables a digital output channel */
    if (device_data.name == std::string("Digital Discovery")) {
        channel = channel - 24;
    }
    FDwfDigitalOutEnableSet(device_data.handle, channel, 0);
    FDwfDigitalOutConfigure(device_data.handle, true);
    state.channel[channel] = false;
    return;
}
