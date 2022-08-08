/* STATIC I/O CONTROL FUNCTIONS: set_mode, get_state, set_state, set_current, set_pull, close */

/* include the header */
#include "static.h"

/* ----------------------------------------------------- */

void Static::set_mode(Device::Data device_data, int channel, bool output) {
    /*
        set a DIO line as input, or as output

        parameters: - device data
                    - selected DIO channel number
                    - True means output, False means input
    */
    // load current state of the output enable buffer
    unsigned int mask = 0;
    FDwfDigitalIOOutputEnableGet(device_data.handle, &mask);
    
    // set bit in mask
    if (output == true) {
        mask |= rotate_left(1, channel);
    }
    else {
        mask &= rotate_left(0xFFFE, channel);
    }
    
    // set the pin to output
    FDwfDigitalIOOutputEnableSet(device_data.handle, mask);
    state.input[channel] = !output;
    state.output[channel] = output;
    if (!output) {
        state.state[channel] = bool(-1);
    }
    return;
}

/* ----------------------------------------------------- */

bool Static::get_state(Device::Data device_data, int channel) {
    /*
        get the state of a DIO line

        parameters: - device data
                    - selected DIO channel number

        returns:    - True if the channel is HIGH, or False, if the channel is LOW
    */
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

void Static::set_state(Device::Data device_data, int channel, bool value) {
    /*
        set a DIO line as input, or as output

        parameters: - device data
                    - selected DIO channel number
                    - True means HIGH, False means LOW
    */
    // load current state of the output state buffer
    unsigned int mask = 0;
    FDwfDigitalIOOutputGet(device_data.handle, &mask);
    
    // set bit in mask
    if (value == true) {
        mask |= rotate_left(1, channel);
    }
    else {
        mask &= rotate_left(0xFFFE, channel);
    }
    
    // set the pin state
    FDwfDigitalIOOutputSet(device_data.handle, mask);
    state.state[channel] = value;
    return;
}

/* ----------------------------------------------------- */

void Static::set_current(Device::Data device_data, int current) {
    /*
        limit the output current of the DIO lines

        parameters: - device data
                    - current limit in mA: possible values are 2, 4, 6, 8, 12 and 16mA
    */
    // discard unrecognized current values
    switch (current) {
        case 2: break;
        case 4: break;
        case 6: break;
        case 8: break;
        case 12: break;
        case 16: break;
        default: current = 16; break;
    }

    // set limit  
    FDwfAnalogIOChannelNodeSet(device_data.handle, 0, 4, double(current));
    state.current = current;
    return;
}

/* ----------------------------------------------------- */

void Static::set_pull(Device::Data device_data, int channel, bool direction) {
    /*
        pull a DIO line up, or down

        parameters: - device data
                    - selected DIO channel number between 0-15
                    - direction: True means HIGH, False means LOW, None means idle
    */
    
    // encode direction
    state.pull[channel] = direction;
    double dir = 0.5;
    if (direction == 1) {
        dir = 1.0;
    }
    else if (direction == 0) {
        dir = 0.0;
    }

    // get pull enable mask
    double mask = 0;
    FDwfAnalogIOChannelNodeGet(device_data.handle, 0, 2, &mask);

    // set bit in mask
    unsigned int mask_ = (unsigned int)(mask);
    if (dir == 0.5) {
        mask_ |= rotate_left(1, channel);
    }
    else {
        mask_ &= rotate_left(0xFFFE, channel);
    }
    
    // set pull enable mask
    FDwfAnalogIOChannelNodeSet(device_data.handle, 0, 2, mask_);
    
    // set direction if necessary
    if (dir != 0.5) {
        // get direction mask
        double mask__ = 0;
        FDwfAnalogIOChannelNodeGet(device_data.handle, 0, 3, &mask__);

        // set bit in mask
        unsigned int mask___ = (unsigned int)(mask__);
        if (dir == 1) {
            mask___ |= rotate_left(1, channel);
        }
        else {
            mask___ &= rotate_left(0xFFFE, channel);
        }
        
        // set direction mask
        FDwfAnalogIOChannelNodeSet(device_data.handle, 0, 3, mask___);
    }

    return;
}

/* ----------------------------------------------------- */

void Static::close(Device::Data device_data) {
    /*
        reset the instrument
    */
    FDwfDigitalIOReset(device_data.handle);
    state.current = 0;
    for (int index = 0; index < 16; index++) {
        state.state[index] = bool(-1);
        state.input[index] = true;
        state.output[index] = false;
        state.pull[index] = bool(-1);
    }
    return;
}

/* ----------------------------------------------------- */

unsigned int Static::rotate_left(unsigned int number, unsigned int position, unsigned int size) {
    return (number << position) | (number >> (size - position));
}
