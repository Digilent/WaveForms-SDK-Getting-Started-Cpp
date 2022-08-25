/* LOGIC ANALYZER CONTROL FUNCTIONS: open, trigger, record, close */

/* include the header */
#include "logic.h"

/* ----------------------------------------------------- */

void wf::Logic::open(Device::Data device_data, double sampling_frequency, int buffer_size) {
    /*
        initialize the logic analyzer

        parameters: - device data
                    - sampling frequency in Hz, default is 100MHz
                    - buffer size, default is 0 (maximum)
    */
    
    //set global variables
    data.sampling_frequency = sampling_frequency;
    data.max_buffer_size = device_data.digital.input.max_buffer_size;

    // get internal clock frequency
    double internal_frequency = 0;
    FDwfDigitalInInternalClockInfo(device_data.handle, &internal_frequency);
    
    // set clock frequency divider (needed for lower frequency input signals)
    FDwfDigitalInDividerSet(device_data.handle, int(internal_frequency / sampling_frequency));
    
    // set 16-bit sample format
    FDwfDigitalInSampleFormatSet(device_data.handle, 16);
    
    // set buffer size
    if (buffer_size == 0 || buffer_size > data.max_buffer_size) {
        buffer_size = data.max_buffer_size;
    }
    data.buffer_size = buffer_size;
    FDwfDigitalInBufferSizeSet(device_data.handle, buffer_size);

    state.on = true;
    state.off = false;
    return;
}

/* ----------------------------------------------------- */

void wf::Logic::trigger(Device::Data device_data, bool enable, int channel, int position, double timeout, bool rising_edge, double length_min, double length_max, int count) {
    /*
        set up triggering

        parameters: - device data
                    - enable - True or False to enable, or disable triggering
                    - channel - the selected DIO line number to use as trigger source
                    - position - prefill size, the default is 0
                    - timeout - auto trigger time, the default is 0
                    - rising_edge - set True for rising edge, False for falling edge, the default is rising edge
                    - length_min - trigger sequence minimum time in seconds, the default is 0
                    - length_max - trigger sequence maximum time in seconds, the default is 20
                    - count - nt count, the default is 1
    */

    // set trigger source to digital I/O lines, or turn it off
    if (enable == true) {
        FDwfDigitalInTriggerSourceSet(device_data.handle, trigsrcDetectorDigitalIn);
        state.trigger = true;
    }
    else {
        FDwfDigitalInTriggerSourceSet(device_data.handle, trigsrcNone);
        state.trigger = false;
        return;
    }
    
    // set starting position and prefill
    position = min(data.buffer_size, max(0, position));
    FDwfDigitalInTriggerPositionSet(device_data.handle, data.buffer_size - position);
    FDwfDigitalInTriggerPrefillSet(device_data.handle, position);

    // set trigger condition
    channel = int(1 << channel);
    if (rising_edge == true) {
        FDwfDigitalInTriggerSet(device_data.handle, channel, 0, 0, 0);
        FDwfDigitalInTriggerResetSet(device_data.handle, 0, 0, 0, channel);
    }
    else {
        FDwfDigitalInTriggerSet(device_data.handle, 0, channel, 0, 0);
        FDwfDigitalInTriggerResetSet(device_data.handle, 0, 0, channel, 0);
    }
    
    // set auto triggering
    FDwfDigitalInTriggerAutoTimeoutSet(device_data.handle, timeout);
    
    // set sequence length to activate trigger
    FDwfDigitalInTriggerLengthSet(device_data.handle, length_min, length_max, 0);

    // set event counter
    FDwfDigitalInTriggerCountSet(device_data.handle, count, 0);
    return;
}

/* ----------------------------------------------------- */

std::vector<unsigned short> wf::Logic::record(Device::Data device_data, int channel) {
    /*
        initialize the logic analyzer

        parameters: - device data
                    - channel - the selected DIO line number

        returns:    - buffer - a list with the recorded logic values
    */
    
    // set up the instrument
    FDwfDigitalInConfigure(device_data.handle, false, true);
    
    // read data to an internal buffer
    while (true) {
        unsigned char status = 0;    // variable to store buffer status
        FDwfDigitalInStatus(device_data.handle, true, &status);
    
        if (status == stsDone) {
            // exit loop when finished
            break;
        }
    }

    // get samples
    std::vector<unsigned short> buffer(data.buffer_size);
    FDwfDigitalInStatusData(device_data.handle, buffer.data(), 2 * data.buffer_size);

    // get channel specific data
    for (int index = 0; index < data.buffer_size; index++) {
        buffer[index] = (buffer[index] & (1 << channel)) >> channel;
    }
    
    return buffer;
}

/* ----------------------------------------------------- */

void wf::Logic::close(Device::Data device_data) {
    /*
        reset the instrument
    */
    FDwfDigitalInReset(device_data.handle);
    state.on = false;
    state.off = true;
    state.trigger = false;
    return;
}

/* ----------------------------------------------------- */

int wf::Logic::min(int a, int b) {
    if (a < b) {
        return a;
    }
    return b;
}

/* ----------------------------------------------------- */

int wf::Logic::max(int a, int b) {
    if (a > b) {
        return a;
    }
    return b;
}
