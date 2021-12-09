/* LOGIC ANALYZER CONTROL FUNCTIONS: open, trigger, record, close */

/* include the header */
#include "logic.h"

/* ----------------------------------------------------- */

void Logic::open(HDWF device_handle, double sampling_frequency, int buffer_size) {
    /*
        initialize the logic analyzer

        parameters: - device handle
                    - sampling frequency in Hz, default is 100MHz
                    - buffer size, default is 4096
    */
    // get internal clock frequency
    double internal_frequency = 0;
    FDwfDigitalInInternalClockInfo(device_handle, &internal_frequency);
    
    // set clock frequency divider (needed for lower frequency input signals)
    FDwfDigitalInDividerSet(device_handle, int(internal_frequency / sampling_frequency));
    
    // set 16-bit sample format
    FDwfDigitalInSampleFormatSet(device_handle, 16);
    
    // set buffer size
    FDwfDigitalInBufferSizeSet(device_handle, buffer_size);
    return;
}

/* ----------------------------------------------------- */

void Logic::trigger(HDWF device_handle, bool enable, int channel, int buffer_size, int position, double timeout, bool rising_edge, double length_min, double length_max, int count) {
    /*
        set up triggering

        parameters: - device handle
                    - enable - True or False to enable, or disable triggering
                    - channel - the selected DIO line number to use as trigger source
                    - buffer size, the default is 4096
                    - position - prefill size, the default is 0
                    - timeout - auto trigger time, the default is 0
                    - rising_edge - set True for rising edge, False for falling edge, the default is rising edge
                    - length_min - trigger sequence minimum time in seconds, the default is 0
                    - length_max - trigger sequence maximum time in seconds, the default is 20
                    - count - nt count, the default is 1
    */
    // set trigger source to digital I/O lines, or turn it off
    if (enable == true) {
        FDwfDigitalInTriggerSourceSet(device_handle, trigsrcDetectorDigitalIn);
    }
    else {
        FDwfDigitalInTriggerSourceSet(device_handle, trigsrcNone);
        return;
    }
    
    // set starting position and prefill
    position = min(buffer_size, max(0, position));
    FDwfDigitalInTriggerPositionSet(device_handle, buffer_size - position);
    FDwfDigitalInTriggerPrefillSet(device_handle, position);

    // set trigger condition
    channel = int(1 << channel);
    if (rising_edge == true) {
        FDwfDigitalInTriggerSet(device_handle, channel, 0, 0, 0);
        FDwfDigitalInTriggerResetSet(device_handle, 0, 0, 0, channel);
    }
    else {
        FDwfDigitalInTriggerSet(device_handle, 0, channel, 0, 0);
        FDwfDigitalInTriggerResetSet(device_handle, 0, 0, channel, 0);
    }
    
    // set auto triggering
    FDwfDigitalInTriggerAutoTimeoutSet(device_handle, timeout);
    
    // set sequence length to activate trigger
    FDwfDigitalInTriggerLengthSet(device_handle, length_min, length_max, 0);

    // set event counter
    FDwfDigitalInTriggerCountSet(device_handle, count, 0);
    return;
}

/* ----------------------------------------------------- */

logic_data Logic::record(HDWF device_handle, int channel, double sampling_frequency, int buffer_size) {
    /*
        initialize the logic analyzer

        parameters: - device handle
                    - channel - the selected DIO line number
                    - sampling frequency in Hz, default is 100MHz
                    - buffer size, default is 4096

        returns:    - buffer - a list with the recorded logic values
                    - time - a list with the time moments for each value in seconds (with the same index as "buffer")
    */
    // set up the instrument
    FDwfDigitalInConfigure(device_handle, false, true);
    
    // read data to an internal buffer
    while (true) {
        unsigned char status = 0;    // variable to store buffer status
        FDwfDigitalInStatus(device_handle, true, &status);
    
        if (status == stsDone) {
            // exit loop when finished
            break;
        }
    }

    // get samples
    unsigned short buffer[buffer_size];
    FDwfDigitalInStatusData(device_handle, buffer, 2 * buffer_size);

    // get channel specific data
    unsigned char channel_buffer[16][buffer_size];
    for (int index = 0; index < buffer_size; index++) {
        for (int bit = 0; bit < 16; bit++) {
            channel_buffer[bit][index] = buffer[index] & (1 << bit);
        }
    }
    
    // calculate aquisition time
    double time[buffer_size];
    for (int index = 0; index < buffer_size; index++) {
        time[index] = index / sampling_frequency;
    }
    
    // return everything
    logic_data data;
    data.buffer = channel_buffer[channel];
    data.time = time;
    return data;
}

/* ----------------------------------------------------- */

void Logic::close(HDWF device_handle) {
    /*
        reset the instrument
    */
    FDwfDigitalInReset(device_handle);
    return;
}
