/* OSCILLOSCOPE CONTROL FUNCTIONS: open, measure, trigger, record, close */

/* include the header */
#include "scope.h"

/* ----------------------------------------------------- */

void wf::Scope::open(Device::Data *device_data, double sampling_frequency, int buffer_size, double offset, double amplitude_range) {
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
    data.max_buffer_size = device_data->analog.input.max_buffer_size;
    // enable all channels
    if (FDwfAnalogInChannelEnableSet(device_data->handle, -1, true) == 0) {
        device.check_error(device_data);
    }
    
    // set offset voltage (in Volts)
    if (FDwfAnalogInChannelOffsetSet(device_data->handle, -1, offset) == 0) {
        device.check_error(device_data);
    }
    
    // set range (maximum signal amplitude in Volts)
    if (FDwfAnalogInChannelRangeSet(device_data->handle, -1, amplitude_range) == 0) {
        device.check_error(device_data);
    }
    
    // set the buffer size (data point in a recording)
    if (buffer_size == 0 || buffer_size > data.max_buffer_size) {
        buffer_size = data.max_buffer_size;
    }
    data.buffer_size = buffer_size;
    if (FDwfAnalogInBufferSizeSet(device_data->handle, buffer_size) == 0) {
        device.check_error(device_data);
    }
    
    // set the acquisition frequency (in Hz)
    if (FDwfAnalogInFrequencySet(device_data->handle, sampling_frequency) == 0) {
        device.check_error(device_data);
    }
    
    // disable averaging (for more info check the documentation)
    if (FDwfAnalogInChannelFilterSet(device_data->handle, -1, filterDecimate) == 0) {
        device.check_error(device_data);
    }
    return;
}

/* ----------------------------------------------------- */

double wf::Scope::measure(Device::Data *device_data, int channel) {
    /*
        measure a voltage

        parameters: - device data
                    - the selected oscilloscope channel (1-2, or 1-4)
        
        returns:    - the measured voltage in Volts
    */
    // set up the instrument
    if (FDwfAnalogInConfigure(device_data->handle, false, false) == 0) {
        device.check_error(device_data);
    }
    
    // read data to an internal buffer
    if (FDwfAnalogInStatus(device_data->handle, false, 0) == 0) {
        device.check_error(device_data);
    }
    
    // extract data from that buffer
    double voltage = 0; // variable to store the measured voltage
    if (FDwfAnalogInStatusSample(device_data->handle, channel - 1, &voltage) == 0) {
        device.check_error(device_data);
    }
    return voltage;
}

/* ----------------------------------------------------- */

void wf::Scope::trigger(Device::Data *device_data, bool enable, const TRIGSRC source, int channel, double timeout, bool edge_rising, double level) {
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
        if (FDwfAnalogInTriggerAutoTimeoutSet(device_data->handle, timeout) == 0) {
            device.check_error(device_data);
        }

        // set trigger source
        if (FDwfAnalogInTriggerSourceSet(device_data->handle, source) == 0) {
            device.check_error(device_data);
        }

        // set trigger channel
        if (source == trigsrcDetectorAnalogIn) {
            channel--;  // decrement analog channel index
        }
        if (FDwfAnalogInTriggerChannelSet(device_data->handle, channel) == 0) {
            device.check_error(device_data);
        }

        // set trigger type
        if (FDwfAnalogInTriggerTypeSet(device_data->handle, trigtypeEdge) == 0) {
            device.check_error(device_data);
        }

        // set trigger level
        if (FDwfAnalogInTriggerLevelSet(device_data->handle, level) == 0) {
            device.check_error(device_data);
        }

        // set trigger edge
        if (edge_rising) {
            // rising edge
            if (FDwfAnalogInTriggerConditionSet(device_data->handle, trigcondRisingPositive) == 0) {
                device.check_error(device_data);
            }
        }
        else {
            // falling edge
            if (FDwfAnalogInTriggerConditionSet(device_data->handle, trigcondFallingNegative) == 0) {
                device.check_error(device_data);
            }
        }
    }
    else {
        // turn off the trigger
        if (FDwfAnalogInTriggerSourceSet(device_data->handle, trigsrcNone) == 0) {
            device.check_error(device_data);
        }
    }
    return;
}

/* ----------------------------------------------------- */

std::vector<double> wf::Scope::record(Device::Data *device_data, int channel) {
    /*
        record an analog signal

        parameters: - device handle
                    - the selected oscilloscope channel (1-2, or 1-4)

        returns:    - buffer - a list with the recorded voltages
    */
    // set up the instrument
    if (FDwfAnalogInConfigure(device_data->handle, false, true) == 0) {
        device.check_error(device_data);
    }
    
    // read data to an internal buffer
    while (true) {
        unsigned char status = 0;   // variable to store buffer status
        if (FDwfAnalogInStatus(device_data->handle, true, &status) == 0) {
            device.check_error(device_data);
        }
    
        // check internal buffer status
        if (status == DwfStateDone) {
            // exit loop when ready
            break;
        }
    }
    
    // copy buffer
    std::vector<double> buffer(data.buffer_size);  // try to create an empty buffer
    if (FDwfAnalogInStatusData(device_data->handle, channel - 1, buffer.data(), data.buffer_size) == 0) {
        device.check_error(device_data);
    }
    return buffer;
}

/* ----------------------------------------------------- */

void wf::Scope::close(Device::Data *device_data) {
    /*
        reset the scope
    */
    if (FDwfAnalogInReset(device_data->handle) == 0) {
        device.check_error(device_data);
    }
    return;
}
