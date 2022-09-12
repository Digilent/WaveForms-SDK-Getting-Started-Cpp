/* STATIC I/O CONTROL FUNCTIONS: set_mode, get_state, set_state, set_current, set_pull, close */

/* include the header */
#include "static.h"

/* ----------------------------------------------------- */

void wf::Static::set_mode(Device::Data device_data, int channel, bool output) {
    /*
        set a DIO line as input, or as output

        parameters: - device data
                    - selected DIO channel number
                    - True means output, False means input
    */
    if (device_data.name == std::string("Digital Discovery")) {
        channel = channel - 24;
    }

    // count the DIO channels
    state.count = min(device_data.digital.input.channel_count, device_data.digital.output.channel_count);

    // load current state of the output enable buffer
    unsigned int mask = 0;
    FDwfDigitalIOOutputEnableGet(device_data.handle, &mask);
    
    // set bit in mask
    if (output == true) {
        mask |= rotate_left(1, channel, state.count);
    }
    else {
        unsigned int bits = (unsigned int)pow(2, state.count) - 2;
        mask &= rotate_left(bits, channel, state.count);
    }
    
    // set the pin to output
    FDwfDigitalIOOutputEnableSet(device_data.handle, mask);
    return;
}

/* ----------------------------------------------------- */

bool wf::Static::get_state(Device::Data device_data, int channel) {
    /*
        get the state of a DIO line

        parameters: - device data
                    - selected DIO channel number

        returns:    - True if the channel is HIGH, or False, if the channel is LOW
    */
    if (device_data.name == std::string("Digital Discovery")) {
        channel = channel - 24;
    }

    // load internal buffer with current state of the pins
    FDwfDigitalIOStatus(device_data.handle);
    
    // get the current state of the pins
    unsigned int data = 0;  // variable for this current state
    FDwfDigitalIOInputStatus(device_data.handle, &data);
    
    // check the required bit
    if (data & (1 << channel) != 0) {
        return true;
    }
    else {
        return false;
    }
}

/* ----------------------------------------------------- */

void wf::Static::set_state(Device::Data device_data, int channel, bool value) {
    /*
        set a DIO line as input, or as output

        parameters: - device data
                    - selected DIO channel number
                    - True means HIGH, False means LOW
    */
    if (device_data.name == std::string("Digital Discovery")) {
        channel = channel - 24;
    }

    // count the DIO channels
    state.count = min(device_data.digital.input.channel_count, device_data.digital.output.channel_count);

    // load current state of the output state buffer
    unsigned int mask = 0;
    FDwfDigitalIOOutputGet(device_data.handle, &mask);
    
    // set bit in mask
    if (value == true) {
        mask |= rotate_left(1, channel, state.count);
    }
    else {
        unsigned int bits = (unsigned int)pow(2, state.count) - 2;
        mask &= rotate_left(bits, channel, state.count);
    }
    
    // set the pin state
    FDwfDigitalIOOutputSet(device_data.handle, mask);
    return;
}

/* ----------------------------------------------------- */

void wf::Static::set_current(Device::Data device_data, double current) {
    /*
        limit the output current of the DIO lines

        parameters: - device data
                    - current limit in mA: possible values are 2, 4, 6, 8, 12 and 16mA
    */
    // search for the digital voltage channel
    for (int channel_index = 0; channel_index < device_data.analog.IO.channel_count; channel_index++) {
        if (device_data.analog.IO.channel_label[channel_index] == std::string("VDD")) {
            state.channel = channel_index;
            break;
        }
    }

    // search for the drive node
    if (state.channel >= 0) {
        for (int node_index = 0; node_index < device_data.analog.IO.node_count[state.channel]; node_index++) {
            if (device_data.analog.IO.node_name[state.channel][node_index] == std::string("Drive")) {
                state.nodes.current = node_index;
                break;
            }
        }
    }

    // set limit
    if (state.channel >= 0 && state.nodes.current >= 0) {
        current = max(min(current, device_data.analog.IO.max_set_range[state.channel][state.nodes.current]), device_data.analog.IO.min_set_range[state.channel][state.nodes.current]);
        FDwfAnalogIOChannelNodeSet(device_data.handle, state.channel, state.nodes.current, current);
    }
    return;
}

/* ----------------------------------------------------- */

void wf::Static::set_pull(Device::Data device_data, int channel, double direction) {
    /*
        pull a DIO line up, or down

        parameters: - device data
                    - selected DIO channel number between 0-15
                    - direction: pull.up, pull.down, or pull.idle
    */
    if (device_data.name == std::string("Digital Discovery")) {
        channel = channel - 24;
    }
    
    // count the DIO channels
    state.count = min(device_data.digital.input.channel_count, device_data.digital.output.channel_count);

    // search for the digital voltage channel
    for (int channel_index = 0; channel_index < device_data.analog.IO.channel_count; channel_index++) {
        if (device_data.analog.IO.channel_label[channel_index] == std::string("VDD")) {
            state.channel = channel_index;
            break;
        }
    }
    
    // search for the pull enable node
    if (state.channel >= 0) {
        for (int node_index = 0; node_index < device_data.analog.IO.node_count[state.channel]; node_index++) {
            if (device_data.analog.IO.node_name[state.channel][node_index] == std::string("DIOPE")) {
                state.nodes.pull_enable = node_index;
                break;
            }
        }
    }

    // search for the pull direction node
    if (state.channel >= 0) {
        for (int node_index = 0; node_index < device_data.analog.IO.node_count[state.channel]; node_index++) {
            if (device_data.analog.IO.node_name[state.channel][node_index] == std::string("DIOPP")) {
                state.nodes.pull_direction = node_index;
                break;
            }
        }
    }

    // search for the weak pull node
    if (state.channel >= 0) {
        for (int node_index = 0; node_index < device_data.analog.IO.node_count[state.channel]; node_index++) {
            if (device_data.analog.IO.node_name[state.channel][node_index] == std::string("DINPP")) {
                state.nodes.pull_weak = node_index;
                break;
            }
        }
    }

    // set pull enable mask
    double mask = 0;
    FDwfAnalogIOChannelNodeGet(device_data.handle, state.channel, state.nodes.pull_enable, &mask);
    unsigned int bitmask = (unsigned int)(mask);
    if (direction == pull.idle) {
        bitmask |= rotate_left(1, channel, state.count);
    }
    else {
        unsigned int bits = (unsigned int)pow(2, state.count) - 2;
        bitmask &= rotate_left(bits, channel, state.count);
    }
    FDwfAnalogIOChannelNodeSet(device_data.handle, state.channel, state.nodes.pull_enable, bitmask);

    // set direction if necessary
    if (direction != pull.idle) {
        // set direction mask
        mask = 0;
        FDwfAnalogIOChannelNodeGet(device_data.handle, state.channel, state.nodes.pull_direction, &mask);
        bitmask = (unsigned int)(mask);
        if (direction == pull.up) {
            bitmask |= rotate_left(1, channel, state.count);
        }
        else {
            unsigned int bits = (unsigned int)pow(2, state.count) - 2;
            bitmask &= rotate_left(bits, channel, state.count);
        }
        FDwfAnalogIOChannelNodeSet(device_data.handle, state.channel, state.nodes.pull_direction, bitmask);
    }
    return;
}

/* ----------------------------------------------------- */

void wf::Static::close(Device::Data device_data) {
    /*
        reset the instrument
    */
    FDwfDigitalIOReset(device_data.handle);
    return;
}

/* ----------------------------------------------------- */

unsigned int wf::Static::rotate_left(unsigned int number, unsigned int position, unsigned int size) {
    return (number << position) | (number >> (size - position));
}
