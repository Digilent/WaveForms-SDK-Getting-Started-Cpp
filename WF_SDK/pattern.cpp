/* PATTERN GENERATOR CONTROL FUNCTIONS: generate, close */

/* include the header */
#include "pattern.h"

/* ----------------------------------------------------- */

void Pattern::generate(HDWF device_handle, int channel, DwfDigitalOutType function, double frequency, double duty_cycle, unsigned char* data, double wait, int repeat, bool trigger_enabled, TRIGSRC trigger_source, bool trigger_edge_rising) {
    /*
        generate a logic signal
        
        parameters: - channel - the selected DIO line number
                    - function - possible: pulse, custom, random
                    - frequency in Hz
                    - duty cycle in percentage, used only if function = pulse, default is 50%
                    - data list, used only if function = custom, default is empty
                    - wait time in seconds, default is 0 seconds
                    - repeat count, default is infinite (0)
                    - trigger_enabled - include/exclude trigger from repeat cycle
                    - trigger_source - possible: none, analog, digital, external[1-4]
                    - trigger_edge_rising - True means rising, False means falling, None means either, default is rising
    */
    // get internal clock frequency
    double internal_frequency = 0;
    FDwfDigitalOutInternalClockInfo(device_handle, &internal_frequency);
    
    // get counter value range
    unsigned int counter_limit = 0;
    FDwfDigitalOutCounterInfo(device_handle, 0, 0, &counter_limit);
    
    // calculate the divider for the given signal frequency
    int divider = 0 - int(-(internal_frequency / frequency) / counter_limit);
    
    // enable the respective channel
    FDwfDigitalOutEnableSet(device_handle, channel, 1);
    
    // set output type
    FDwfDigitalOutTypeSet(device_handle, channel, function);
    
    // set frequency
    FDwfDigitalOutDividerSet(device_handle, channel, divider);
    
    // set wait time
    FDwfDigitalOutWaitSet(device_handle, wait);
    
    // set repeat count
    FDwfDigitalOutRepeatSet(device_handle, repeat);
    
    // enable triggering
    FDwfDigitalOutRepeatTriggerSet(device_handle, int(trigger_enabled));
    
    if (trigger_enabled == true) {
        // set trigger source
        FDwfDigitalOutTriggerSourceSet(device_handle, trigger_source);
    
        // set trigger slope
        if (trigger_edge_rising == true) {
            // rising edge
            FDwfDigitalOutTriggerSlopeSet(device_handle, DwfTriggerSlopeRise);
        }
        else if (trigger_edge_rising == false) {
            // falling edge
            FDwfDigitalOutTriggerSlopeSet(device_handle, DwfTriggerSlopeFall);
        }
        else {
            // either edge
            FDwfDigitalOutTriggerSlopeSet(device_handle, DwfTriggerSlopeEither);
        }
    }

    // set PWM signal duty cycle
    if (function == DwfDigitalOutTypePulse) {
        // calculate counter steps to get the required frequency
        int steps = int(round(internal_frequency / frequency / divider));
        // calculate steps for low and high parts of the period
        int high_steps = int(steps * duty_cycle / 100);
        int low_steps = steps - high_steps;
        FDwfDigitalOutCounterSet(device_handle, channel, low_steps, high_steps);
    }
    
    // load custom signal data
    else if (function == DwfDigitalOutTypeCustom) {
        // format data
        int length = sizeof(data) / sizeof(data[0]);
        int buffer_length = (length + 7) >> 3;
        unsigned char buffer[buffer_length];
        for (int index = 0; index < length; index++) {
            buffer[index] = 0;
            if (data[index] != 0) {
                buffer[index >> 3] |= 1 << (index & 7);
            }
        }
    
        // load data
        FDwfDigitalOutDataSet(device_handle, channel, &buffer, length);
    }
    
    // start generating the signal
    FDwfDigitalOutConfigure(device_handle, true);
    return;
}

/* ----------------------------------------------------- */

void Pattern::close(HDWF device_handle) {
    /*
        reset the instrument
    */
    FDwfDigitalOutReset(device_handle);
    return;
}
