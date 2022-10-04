/* STATIC I/O CONTROL FUNCTIONS: set_mode, get_state, set_state, set_current, set_pull, close */

/* include the header */
#include "static.h"

/* ----------------------------------------------------- */

void wf::Static::set_mode(Device::Data *device_data, int channel, bool output) {
    /*
        set a DIO line as input, or as output

        parameters: - device data
                    - selected DIO channel number
                    - True means output, False means input
    */
    if (device_data->name == std::string("Digital Discovery")) {
        channel = channel - 24;
    }

    // count the DIO channels
    data.count = tools.min(device_data->digital.input.channel_count, device_data->digital.output.channel_count);

    // load current state of the output enable buffer
    unsigned int mask = 0;
    if (FDwfDigitalIOOutputEnableGet(device_data->handle, &mask) == 0) {
        device.check_error(device_data);
    }
    
    // set bit in mask
    if (output == true) {
        mask |= rotate_left(1, channel, data.count);
    }
    else {
        unsigned int bits = (unsigned int)pow(2, data.count) - 2;
        mask &= rotate_left(bits, channel, data.count);
    }
    
    // set the pin to output
    if (FDwfDigitalIOOutputEnableSet(device_data->handle, mask) == 0) {
        device.check_error(device_data);
    }
    return;
}

/* ----------------------------------------------------- */

bool wf::Static::get_state(Device::Data *device_data, int channel) {
    /*
        get the state of a DIO line

        parameters: - device data
                    - selected DIO channel number

        returns:    - True if the channel is HIGH, or False, if the channel is LOW
    */
    if (device_data->name == std::string("Digital Discovery")) {
        channel = channel - 24;
    }

    // load internal buffer with current state of the pins
    if (FDwfDigitalIOStatus(device_data->handle) == 0) {
        device.check_error(device_data);
    }
    
    // get the current state of the pins
    unsigned int data = 0;  // variable for this current state
    if (FDwfDigitalIOInputStatus(device_data->handle, &data) == 0) {
        device.check_error(device_data);
    }
    
    // check the required bit
    if (data & (1 << channel) != 0) {
        return true;
    }
    else {
        return false;
    }
}

/* ----------------------------------------------------- */

void wf::Static::set_state(Device::Data *device_data, int channel, bool value) {
    /*
        set a DIO line as input, or as output

        parameters: - device data
                    - selected DIO channel number
                    - True means HIGH, False means LOW
    */
    if (device_data->name == std::string("Digital Discovery")) {
        channel = channel - 24;
    }

    // count the DIO channels
    data.count = tools.min(device_data->digital.input.channel_count, device_data->digital.output.channel_count);

    // load current state of the output state buffer
    unsigned int mask = 0;
    if (FDwfDigitalIOOutputGet(device_data->handle, &mask) == 0) {
        device.check_error(device_data);
    }
    
    // set bit in mask
    if (value == true) {
        mask |= rotate_left(1, channel, data.count);
    }
    else {
        unsigned int bits = (unsigned int)pow(2, data.count) - 2;
        mask &= rotate_left(bits, channel, data.count);
    }
    
    // set the pin state
    if (FDwfDigitalIOOutputSet(device_data->handle, mask) == 0) {
        device.check_error(device_data);
    }
    return;
}

/* ----------------------------------------------------- */

void wf::Static::set_current(Device::Data *device_data, double current) {
    /*
        limit the output current of the DIO lines

        parameters: - device data
                    - current limit in mA: possible values are 2, 4, 6, 8, 12 and 16mA
    */
    // search for the digital voltage channel
    for (int channel_index = 0; channel_index < device_data->analog.IO.channel_count; channel_index++) {
        if (device_data->analog.IO.channel_label[channel_index] == std::string("VDD")) {
            data.channel = channel_index;
            break;
        }
    }

    // search for the drive node
    if (data.channel >= 0) {
        for (int node_index = 0; node_index < device_data->analog.IO.node_count[data.channel]; node_index++) {
            if (device_data->analog.IO.node_name[data.channel][node_index] == std::string("Drive")) {
                data.nodes.current = node_index;
                break;
            }
        }
    }

    // set limit
    if (data.channel >= 0 && data.nodes.current >= 0) {
        current = tools.max(tools.min(current, device_data->analog.IO.max_set_range[data.channel][data.nodes.current]), device_data->analog.IO.min_set_range[data.channel][data.nodes.current]);
        if (FDwfAnalogIOChannelNodeSet(device_data->handle, data.channel, data.nodes.current, current) == 0) {
            device.check_error(device_data);
        }
    }
    return;
}

/* ----------------------------------------------------- */

void wf::Static::set_pull(Device::Data *device_data, int channel, double direction) {
    /*
        pull a DIO line up, or down

        parameters: - device data
                    - selected DIO channel number between 0-15
                    - direction: pull.up, pull.down, or pull.idle
    */
    if (device_data->name == std::string("Digital Discovery")) {
        channel = channel - 24;
    }
    
    // count the DIO channels
    data.count = tools.min(device_data->digital.input.channel_count, device_data->digital.output.channel_count);

    // search for the digital voltage channel
    for (int channel_index = 0; channel_index < device_data->analog.IO.channel_count; channel_index++) {
        if (device_data->analog.IO.channel_label[channel_index] == std::string("VDD")) {
            data.channel = channel_index;
            break;
        }
    }
    
    // search for the pull enable node
    if (data.channel >= 0) {
        for (int node_index = 0; node_index < device_data->analog.IO.node_count[data.channel]; node_index++) {
            if (device_data->analog.IO.node_name[data.channel][node_index] == std::string("DIOPE")) {
                data.nodes.pull_enable = node_index;
                break;
            }
        }
    }

    // search for the pull direction node
    if (data.channel >= 0) {
        for (int node_index = 0; node_index < device_data->analog.IO.node_count[data.channel]; node_index++) {
            if (device_data->analog.IO.node_name[data.channel][node_index] == std::string("DIOPP")) {
                data.nodes.pull_direction = node_index;
                break;
            }
        }
    }

    // search for the weak pull node
    if (data.channel >= 0) {
        for (int node_index = 0; node_index < device_data->analog.IO.node_count[data.channel]; node_index++) {
            if (device_data->analog.IO.node_name[data.channel][node_index] == std::string("DINPP")) {
                data.nodes.pull_weak = node_index;
                break;
            }
        }
    }

    // set pull enable mask
    double mask = 0;
    if (FDwfAnalogIOChannelNodeGet(device_data->handle, data.channel, data.nodes.pull_enable, &mask) == 0) {
        device.check_error(device_data);
    }
    unsigned int bitmask = (unsigned int)(mask);
    if (direction == pull.idle) {
        bitmask |= rotate_left(1, channel, data.count);
    }
    else {
        unsigned int bits = (unsigned int)pow(2, data.count) - 2;
        bitmask &= rotate_left(bits, channel, data.count);
    }
    if (FDwfAnalogIOChannelNodeSet(device_data->handle, data.channel, data.nodes.pull_enable, bitmask) == 0) {
        device.check_error(device_data);
    }

    // set direction if necessary
    if (direction != pull.idle) {
        // set direction mask
        mask = 0;
        if (FDwfAnalogIOChannelNodeGet(device_data->handle, data.channel, data.nodes.pull_direction, &mask) == 0) {
            device.check_error(device_data);
        }
        bitmask = (unsigned int)(mask);
        if (direction == pull.up) {
            bitmask |= rotate_left(1, channel, data.count);
        }
        else {
            unsigned int bits = (unsigned int)pow(2, data.count) - 2;
            bitmask &= rotate_left(bits, channel, data.count);
        }
        if (FDwfAnalogIOChannelNodeSet(device_data->handle, data.channel, data.nodes.pull_direction, bitmask) == 0) {
            device.check_error(device_data);
        }
    }
    return;
}

/* ----------------------------------------------------- */

void wf::Static::close(Device::Data *device_data) {
    /*
        reset the instrument
    */
    if (FDwfDigitalIOReset(device_data->handle) == 0) {
        device.check_error(device_data);
    }
    return;
}

/* ----------------------------------------------------- */

unsigned int wf::Static::rotate_left(unsigned int number, unsigned int position, unsigned int size) {
    return (number << position) | (number >> (size - position));
}
