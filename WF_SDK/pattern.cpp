/* PATTERN GENERATOR CONTROL FUNCTIONS: generate, close */

/* include the header */
#include "pattern.h"

/* ----------------------------------------------------- */

void wf::Pattern::generate(Device::Data *device_data, int channel, DwfDigitalOutType function, double frequency, double duty_cycle, std::vector<unsigned short> data, double wait, int repeat, int run_time, DwfDigitalOutIdle idle, bool trigger_enabled, TRIGSRC trigger_source, bool trigger_edge_rising) {
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
    if (device_data->name == std::string("Digital Discovery")) {
        channel = channel - 24;
    }
    
    // get internal clock frequency
    double internal_frequency = 0;
    if (FDwfDigitalOutInternalClockInfo(device_data->handle, &internal_frequency) == 0) {
        device.check_error(device_data);
    }
    
    // get counter value range
    unsigned int counter_limit = 0;
    if (FDwfDigitalOutCounterInfo(device_data->handle, 0, 0, &counter_limit) == 0) {
        device.check_error(device_data);
    }
    
    // calculate the divider for the given signal frequency
    int divider;
    if (function == DwfDigitalOutTypePulse) {
        divider = int(ceil(internal_frequency / frequency / counter_limit));
    }
    else {
        divider = int(internal_frequency / frequency);
    }
    
    // enable the respective channel
    if (FDwfDigitalOutEnableSet(device_data->handle, channel, 1) == 0) {
        device.check_error(device_data);
    }
    
    // set output type
    if (FDwfDigitalOutTypeSet(device_data->handle, channel, function) == 0) {
        device.check_error(device_data);
    }
    
    // set frequency
    if (FDwfDigitalOutDividerSet(device_data->handle, channel, divider) == 0) {
        device.check_error(device_data);
    }

    // set idle state
    if (FDwfDigitalOutIdleSet(device_data->handle, channel, idle) == 0) {
        device.check_error(device_data);
    }

    // calculate run length
    if (run_time < 0) {
        run_time = data.size() / frequency;
    }
    if (FDwfDigitalOutRunSet(device_data->handle, run_time) == 0) {
        device.check_error(device_data);
    }
    
    // set wait time
    if (FDwfDigitalOutWaitSet(device_data->handle, wait) == 0) {
        device.check_error(device_data);
    }
    
    // set repeat count
    if (FDwfDigitalOutRepeatSet(device_data->handle, repeat) == 0) {
        device.check_error(device_data);
    }
    
    // enable triggering
    if (FDwfDigitalOutRepeatTriggerSet(device_data->handle, int(trigger_enabled)) == 0) {
        device.check_error(device_data);
    }
    
    if (trigger_enabled == true) {
        // set trigger source
        if (FDwfDigitalOutTriggerSourceSet(device_data->handle, trigger_source) == 0) {
            device.check_error(device_data);
        }
    
        // set trigger slope
        if (trigger_edge_rising == true) {
            // rising edge
            if (FDwfDigitalOutTriggerSlopeSet(device_data->handle, DwfTriggerSlopeRise) == 0) {
                device.check_error(device_data);
            }
        }
        else if (trigger_edge_rising == false) {
            // falling edge
            if (FDwfDigitalOutTriggerSlopeSet(device_data->handle, DwfTriggerSlopeFall) == 0) {
                device.check_error(device_data);
            }
        }
        else {
            // either edge
            if (FDwfDigitalOutTriggerSlopeSet(device_data->handle, DwfTriggerSlopeEither) == 0) {
                device.check_error(device_data);
            }
        }
    }

    // set PWM signal duty cycle
    if (function == DwfDigitalOutTypePulse) {
        // calculate counter steps to get the required frequency
        int steps = int(round(internal_frequency / frequency / divider));
        // calculate steps for low and high parts of the period
        int high_steps = int(steps * duty_cycle / 100);
        int low_steps = steps - high_steps;
        if (FDwfDigitalOutCounterSet(device_data->handle, channel, low_steps, high_steps) == 0) {
            device.check_error(device_data);
        }
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
        if (FDwfDigitalOutDataSet(device_data->handle, channel, buffer.data(), buffer.size()) == 0) {
            device.check_error(device_data);
        }
    }
    
    // start generating the signal
    if (FDwfDigitalOutConfigure(device_data->handle, true) == 0) {
        device.check_error(device_data);
    }
    return;
}

/* ----------------------------------------------------- */

void wf::Pattern::close(Device::Data *device_data) {
    /*
        reset the instrument
    */
    if (FDwfDigitalOutReset(device_data->handle) == 0) {
        device.check_error(device_data);
    }
    return;
}

/* ----------------------------------------------------- */

void wf::Pattern::enable(Device::Data *device_data, int channel) {
    /* enables a digital output channel */
    if (device_data->name == std::string("Digital Discovery")) {
        channel = channel - 24;
    }
    if (FDwfDigitalOutEnableSet(device_data->handle, channel, 1) == 0) {
        device.check_error(device_data);
    }
    if (FDwfDigitalOutConfigure(device_data->handle, true) == 0) {
        device.check_error(device_data);
    }
    return;
}

/* ----------------------------------------------------- */

void wf::Pattern::disable(Device::Data *device_data, int channel) {
    /* disables a digital output channel */
    if (device_data->name == std::string("Digital Discovery")) {
        channel = channel - 24;
    }
    if (FDwfDigitalOutEnableSet(device_data->handle, channel, 0) == 0) {
        device.check_error(device_data);
    }
    if (FDwfDigitalOutConfigure(device_data->handle, true) == 0) {
        device.check_error(device_data);
    }
    return;
}
