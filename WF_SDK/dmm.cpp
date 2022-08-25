/* DIGITAL MULTIMETER CONTROL FUNCTIONS: open, measure, close */

/* include the header */
#include "dmm.h"

/* ----------------------------------------------------- */

void wf::DMM::open(Device::Data device_data) {
    /*
        initialize the digital multimeter
    */
    // enable the DMM
    for (int channel_index = 0; channel_index < device_data.analog.IO.channel_count; channel_index++) {
        if (device_data.analog.IO.channel_label[channel_index] == std::string("DMM")) {
            state.channel = channel_index;
            break;
        }
    }
    if (state.channel >= 0) {
        for (int node_index = 0; node_index < device_data.analog.IO.node_count[state.channel]; node_index++) {
            if (device_data.analog.IO.node_name[state.channel][node_index] == std::string("Enable")) {
                state.nodes.enable = node_index;
            }
            else if (device_data.analog.IO.node_name[state.channel][node_index] == std::string("Mode")) {
                state.nodes.mode = node_index;
            }
            else if (device_data.analog.IO.node_name[state.channel][node_index] == std::string("Range")) {
                state.nodes.range = node_index;
            }
            else if (device_data.analog.IO.node_name[state.channel][node_index] == std::string("Meas")) {
                state.nodes.meas = node_index;
            }
            else if (device_data.analog.IO.node_name[state.channel][node_index] == std::string("Raw")) {
                state.nodes.raw = node_index;
            }
            else if (device_data.analog.IO.node_name[state.channel][node_index] == std::string("Input")) {
                state.nodes.input = node_index;
            }
        }
    }
    if (state.channel >= 0 && state.nodes.enable >= 0) {
        FDwfAnalogIOChannelNodeSet(device_data.handle, state.channel, state.nodes.enable, double(1.0));
        state.on = true;
        state.off = false;
    }
    return;
}

/* ----------------------------------------------------- */

double wf::DMM::measure(Device::Data device_data, DwfDmm mode, double range, bool high_impedance) {
    /*
        measure a voltage/current/resistance/continuity/temperature

        parameters: - device data
                    - mode: "ac/dc_voltage", "ac/dc_low/high_current", "resistance", "continuity", "diode", "temperature"
                    - range: voltage/current/resistance/temperature range, 0 means auto, default is auto
                    - high_impedance: input impedance for DC voltage measurement, False means 10MΩ, True means 10GΩ, default is 10MΩ
        
        returns:    - the measured value in V/A/Ω/°C, or -1 on error
    */
    // set input impedance
    if (state.channel >= 0 && state.nodes.input >= 0) {
        if (high_impedance == true) {
            FDwfAnalogIOChannelNodeSet(device_data.handle, state.channel, state.nodes.input, double(1.0));
        }
        else {
            FDwfAnalogIOChannelNodeSet(device_data.handle, state.channel, state.nodes.input, double(0.0));
        }
    }

    // set mode
    if (state.channel >= 0 && state.nodes.mode >= 0) {
        FDwfAnalogIOChannelNodeSet(device_data.handle, state.channel, state.nodes.mode, mode);
    }
        
    // set range
    if (state.channel >= 0 && state.nodes.range >= 0) {
        FDwfAnalogIOChannelNodeSet(device_data.handle, state.channel, state.nodes.range, range);
    }

    // fetch analog I/O status
    if (FDwfAnalogIOStatus(device_data.handle) == 0) {
        // signal error
        return double(-1.0);
    }

    // get reading
    double measurement = 0;
    if (state.channel >= 0 && state.nodes.meas >= 0) {
        FDwfAnalogIOChannelNodeStatus(device_data.handle, state.channel, state.nodes.meas, &measurement);
    }
    return measurement;
}

/* ----------------------------------------------------- */

void wf::DMM::close(Device::Data device_data) {
    /*
        reset the instrument
    */
    // disable the DMM
    if (state.channel >= 0 && state.nodes.enable >= 0) {
        FDwfAnalogIOChannelNodeSet(device_data.handle, state.channel, state.nodes.enable, double(0));
        state.on = false;
        state.off = true;
    }
    // reset the instrument
    FDwfAnalogIOReset(device_data.handle);
    return;
}
