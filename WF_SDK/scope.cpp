/* OSCILLOSCOPE CONTROL FUNCTIONS: open, measure, trigger, record, close */

/* include the header */
#include "scope.h"

/* ----------------------------------------------------- */

void wf::Scope::open(Device::Data device_data, double sampling_frequency, int buffer_size, double offset, double amplitude_range) {
    /*
        initialize the oscilloscope

        parameters: - device data
                    - sampling frequency in Hz, default is 20MHz
                    - buffer size, default is 0 (maximum)
                    - offset voltage in Volts, default is 0V
                    - amplitude range in Volts, default is ±5V
    */
    // set global variables
    data.sampling_frequency = sampling_frequency;
    data.max_buffer_size = device_data.analog.input.max_buffer_size;
    // enable all channels
    FDwfAnalogInChannelEnableSet(device_data.handle, -1, true);
    
    // set offset voltage (in Volts)
    FDwfAnalogInChannelOffsetSet(device_data.handle, -1, offset);
    
    // set range (maximum signal amplitude in Volts)
    FDwfAnalogInChannelRangeSet(device_data.handle, -1, amplitude_range);
    
    // set the buffer size (data point in a recording)
    if (buffer_size == 0 || buffer_size > data.max_buffer_size) {
        buffer_size = data.max_buffer_size;
    }
    data.buffer_size = buffer_size;
    FDwfAnalogInBufferSizeSet(device_data.handle, buffer_size);
    
    // set the acquisition frequency (in Hz)
    FDwfAnalogInFrequencySet(device_data.handle, sampling_frequency);
    
    // disable averaging (for more info check the documentation)
    FDwfAnalogInChannelFilterSet(device_data.handle, -1, filterDecimate);
    
    state.on = true;
    state.off = false;
    return;
}

/* ----------------------------------------------------- */

double wf::Scope::measure(Device::Data device_data, int channel) {
    /*
        measure a voltage

        parameters: - device data
                    - the selected oscilloscope channel (1-2, or 1-4)
        
        returns:    - the measured voltage in Volts
    */
    // set up the instrument
    FDwfAnalogInConfigure(device_data.handle, false, false);
    
    // read data to an internal buffer
    FDwfAnalogInStatus(device_data.handle, false, 0);
    
    // extract data from that buffer
    double voltage = 0; // variable to store the measured voltage
    FDwfAnalogInStatusSample(device_data.handle, channel - 1, &voltage);
    return voltage;
}

/* ----------------------------------------------------- */

void wf::Scope::trigger(Device::Data device_data, bool enable, const TRIGSRC source, int channel, double timeout, bool edge_rising, double level) {
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
        FDwfAnalogInTriggerAutoTimeoutSet(device_data.handle, timeout);

        // set trigger source
        FDwfAnalogInTriggerSourceSet(device_data.handle, source);

        // set trigger channel
        if (source == trigsrcDetectorAnalogIn) {
            channel--;  // decrement analog channel index
        }
        FDwfAnalogInTriggerChannelSet(device_data.handle, channel);

        // set trigger type
        FDwfAnalogInTriggerTypeSet(device_data.handle, trigtypeEdge);

        // set trigger level
        FDwfAnalogInTriggerLevelSet(device_data.handle, level);

        // set trigger edge
        if (edge_rising) {
            // rising edge
            FDwfAnalogInTriggerConditionSet(device_data.handle, trigcondRisingPositive);
        }
        else {
            // falling edge
            FDwfAnalogInTriggerConditionSet(device_data.handle, trigcondFallingNegative);
        }
        state.trigger = true;
    }
    else {
        // turn off the trigger
        FDwfAnalogInTriggerSourceSet(device_data.handle, trigsrcNone);
        state.trigger = false;
    }
    return;
}

/* ----------------------------------------------------- */

std::vector<double> wf::Scope::record(Device::Data device_data, int channel) {
    /*
        record an analog signal

        parameters: - device handle
                    - the selected oscilloscope channel (1-2, or 1-4)

        returns:    - buffer - a list with the recorded voltages
    */
    // set up the instrument
    FDwfAnalogInConfigure(device_data.handle, false, true);
    
    // read data to an internal buffer
    while (true) {
        unsigned char status = 0;   // variable to store buffer status
        FDwfAnalogInStatus(device_data.handle, true, &status);
    
        // check internal buffer status
        if (status == DwfStateDone) {
            // exit loop when ready
            break;
        }
    }
    
    // copy buffer
    std::vector<double> buffer(data.buffer_size);  // try to create an empty buffer
    FDwfAnalogInStatusData(device_data.handle, channel - 1, buffer.data(), data.buffer_size);
    return buffer;
}

/* ----------------------------------------------------- */

void wf::Scope::close(Device::Data device_data) {
    /*
        reset the scope
    */
    FDwfAnalogInReset(device_data.handle);
    state.on = false;
    state.off = true;
    state.trigger = false;
    return;
}
