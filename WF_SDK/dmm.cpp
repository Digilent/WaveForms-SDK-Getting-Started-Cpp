/* DIGITAL MULTIMETER CONTROL FUNCTIONS: open, measure, close */

/* include the header */
#include "dmm.h"

/* ----------------------------------------------------- */

void wf::DMM::open(Device::Data *device_data) {
    /*
        initialize the digital multimeter
    */
    // enable the DMM
    for (int channel_index = 0; channel_index < device_data->analog.IO.channel_count; channel_index++) {
        if (device_data->analog.IO.channel_label[channel_index] == std::string("DMM")) {
            data.channel = channel_index;
            break;
        }
    }
    if (data.channel >= 0) {
        for (int node_index = 0; node_index < device_data->analog.IO.node_count[data.channel]; node_index++) {
            if (device_data->analog.IO.node_name[data.channel][node_index] == std::string("Enable")) {
                data.nodes.enable = node_index;
            }
            else if (device_data->analog.IO.node_name[data.channel][node_index] == std::string("Mode")) {
                data.nodes.mode = node_index;
            }
            else if (device_data->analog.IO.node_name[data.channel][node_index] == std::string("Range")) {
                data.nodes.range = node_index;
            }
            else if (device_data->analog.IO.node_name[data.channel][node_index] == std::string("Meas")) {
                data.nodes.meas = node_index;
            }
            else if (device_data->analog.IO.node_name[data.channel][node_index] == std::string("Raw")) {
                data.nodes.raw = node_index;
            }
            else if (device_data->analog.IO.node_name[data.channel][node_index] == std::string("Input")) {
                data.nodes.input = node_index;
            }
        }
    }
    if (data.channel >= 0 && data.nodes.enable >= 0) {
        if (FDwfAnalogIOChannelNodeSet(device_data->handle, data.channel, data.nodes.enable, double(1.0)) == 0) {
            device.check_error(device_data);
        }
    }
    return;
}

/* ----------------------------------------------------- */

double wf::DMM::measure(Device::Data *device_data, DwfDmm mode, double range, bool high_impedance) {
    /*
        measure a voltage/current/resistance/continuity/temperature

        parameters: - device data
                    - mode: "ac/dc_voltage", "ac/dc_low/high_current", "resistance", "continuity", "diode", "temperature"
                    - range: voltage/current/resistance/temperature range, 0 means auto, default is auto
                    - high_impedance: input impedance for DC voltage measurement, False means 10MΩ, True means 10GΩ, default is 10MΩ
        
        returns:    - the measured value in V/A/Ω/°C, or -1 on error
    */
    // set input impedance
    if (data.channel >= 0 && data.nodes.input >= 0) {
        if (high_impedance == true) {
            if (FDwfAnalogIOChannelNodeSet(device_data->handle, data.channel, data.nodes.input, double(1.0)) == 0) {
                device.check_error(device_data);
            }
        }
        else {
            if (FDwfAnalogIOChannelNodeSet(device_data->handle, data.channel, data.nodes.input, double(0.0)) == 0) {
                device.check_error(device_data);
            }
        }
    }

    // set mode
    if (data.channel >= 0 && data.nodes.mode >= 0) {
        if (FDwfAnalogIOChannelNodeSet(device_data->handle, data.channel, data.nodes.mode, mode) == 0) {
            device.check_error(device_data);
        }
    }
        
    // set range
    if (data.channel >= 0 && data.nodes.range >= 0) {
        if (FDwfAnalogIOChannelNodeSet(device_data->handle, data.channel, data.nodes.range, range) == 0) {
            device.check_error(device_data);
        }
    }

    // fetch analog I/O status
    if (FDwfAnalogIOStatus(device_data->handle) == 0) {
        // signal error
        device.check_error(device_data);
        return double(-1.0);
    }

    // get reading
    double measurement = 0;
    if (data.channel >= 0 && data.nodes.meas >= 0) {
        if (FDwfAnalogIOChannelNodeStatus(device_data->handle, data.channel, data.nodes.meas, &measurement) == 0) {
            device.check_error(device_data);
        }
    }
    return measurement;
}

/* ----------------------------------------------------- */

void wf::DMM::close(Device::Data *device_data) {
    /*
        reset the instrument
    */
    // disable the DMM
    if (data.channel >= 0 && data.nodes.enable >= 0) {
        if (FDwfAnalogIOChannelNodeSet(device_data->handle, data.channel, data.nodes.enable, double(0)) == 0) {
            device.check_error(device_data);
        }
    }
    // reset the instrument
    if (FDwfAnalogIOReset(device_data->handle) == 0) {
        device.check_error(device_data);
    }
    return;
}
