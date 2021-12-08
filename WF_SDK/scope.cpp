/* OSCILLOSCOPE CONTROL FUNCTIONS: open, measure, trigger, record, close */

/* include the header */
#include "scope.h"

/* ----------------------------------------------------- */

void Scope::open(HDWF device_handle, double sampling_frequency = 20e06, int buffer_size = 8192, double offset = 0, double amplitude_range = 5) {
    /*
        initialize the oscilloscope

        parameters: - device handle
                    - sampling frequency in Hz, default is 20MHz
                    - buffer size, default is 8192
                    - offset voltage in Volts, default is 0V
                    - amplitude range in Volts, default is ±5V
    */
    // enable all channels
    FDwfAnalogInChannelEnableSet(device_handle, 0, true);
    
    // set offset voltage (in Volts)
    FDwfAnalogInChannelOffsetSet(device_handle, 0, offset);
    
    // set range (maximum signal amplitude in Volts)
    FDwfAnalogInChannelRangeSet(device_handle, 0, amplitude_range);
    
    // set the buffer size (data point in a recording)
    FDwfAnalogInBufferSizeSet(device_handle, buffer_size);
    
    // set the acquisition frequency (in Hz)
    FDwfAnalogInFrequencySet(device_handle, sampling_frequency);
    
    // disable averaging (for more info check the documentation)
    FDwfAnalogInChannelFilterSet(device_handle, -1, filterDecimate);
    return;
}

/* ----------------------------------------------------- */

double Scope::measure(HDWF device_handle, int channel) {
    /*
        measure a voltage

        parameters: - device handler
                    - the selected oscilloscope channel (1-2, or 1-4)
        
        returns:    - the measured voltage in Volts
    */
    // set up the instrument
    FDwfAnalogInConfigure(device_handle, false, false);
    
    // read data to an internal buffer
    FDwfAnalogInStatus(device_handle, false, 0);
    
    // extract data from that buffer
    double voltage = 0; // variable to store the measured voltage
    FDwfAnalogInStatusSample(device_handle, channel - 1, &voltage);
    return voltage;
}

/* ----------------------------------------------------- */

void Scope::trigger(HDWF device_handle, bool enable, const TRIGSRC source = trigger_source.none, int channel = 1, double timeout = 0, bool edge_rising = true, double level = 0) {
    /*
        set up triggering

        parameters: - device handle
                    - enable / disable triggering with True/False
                    - trigger source - possible: none, analog, digital, external[1-4]
                    - trigger channel - possible options: 1-4 for analog, or 0-15 for digital
                    - auto trigger timeout in seconds, default is 0
                    - trigger edge rising - True means rising, False means falling, default is rising
                    - trigger level in Volts, default is 0V
    */
    if (enable && source != trigsrcNone) {
        // enable/disable auto triggering
        FDwfAnalogInTriggerAutoTimeoutSet(device_handle, timeout);

        // set trigger source
        FDwfAnalogInTriggerSourceSet(device_handle, source);

        // set trigger channel
        if (source == trigsrcDetectorAnalogIn) {
            channel--;  // decrement analog channel index
        }
        FDwfAnalogInTriggerChannelSet(device_handle, channel);

        // set trigger type
        FDwfAnalogInTriggerTypeSet(device_handle, trigtypeEdge);

        // set trigger level
        FDwfAnalogInTriggerLevelSet(device_handle, level);

        // set trigger edge
        if (edge_rising) {
            // rising edge
            FDwfAnalogInTriggerConditionSet(device_handle, trigcondRisingPositive);
        }
        else {
            // falling edge
            FDwfAnalogInTriggerConditionSet(device_handle, trigcondFallingNegative);
        }
    }
    else {
        // turn off the trigger
        FDwfAnalogInTriggerSourceSet(device_handle, trigsrcNone);
    }
    return;
}

/* ----------------------------------------------------- */

scope_data Scope::record(HDWF device_handle, int channel, double sampling_frequency=20e06, int buffer_size=8192) {
    /*
        record an analog signal

        parameters: - device handle
                    - the selected oscilloscope channel (1-2, or 1-4)
                    - sampling frequency in Hz, default is 20MHz
                    - buffer size, default is 8192

        returns:    - buffer - a list with the recorded voltages
                    - time - a list with the time moments for each voltage in seconds (with the same index as "buffer")
    */
    // set up the instrument
    FDwfAnalogInConfigure(device_handle, false, true);
    
    // read data to an internal buffer
    while (true) {
        unsigned char status = 0;   // variable to store buffer status
        FDwfAnalogInStatus(device_handle, true, &status);
    
        // check internal buffer status
        if (status == DwfStateDone) {
            // exit loop when ready
            break;
        }
    }
    
    // copy buffer
    double buffer[buffer_size]; // create an empty buffer
    FDwfAnalogInStatusData(device_handle, channel - 1, buffer, buffer_size);
    
    // calculate aquisition time
    double time[buffer_size];
    for (int index = 0; index < buffer_size; index++) {
        time[index] = index / sampling_frequency;
    }
    
    // return everything
    scope_data data;
    data.buffer = buffer;
    data.time = time;
    return data;
}

/* ----------------------------------------------------- */

void Scope::close(HDWF device_handle) {
    /*
        reset the scope
    */
    FDwfAnalogInReset(device_handle);
    return;
}
