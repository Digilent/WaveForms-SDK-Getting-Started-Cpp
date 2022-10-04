/* POWER SUPPLIES CONTROL FUNCTIONS: switch_, close */

/* include the header */
#include "supplies.h"

/* ----------------------------------------------------- */

void wf::Supplies::switch_(Device::Data *device_data, Data supplies_data) {
    /*
        turn the power supplies on/off

        parameters: - device data
                    - class containing supplies data:
                        - master_state
                        - state and/or positive_state and negative_state
                        - voltage and/or positive_voltage and negative_voltage
                        - current and/or positive_current and negative_current
    */
    
    //find the positive supply
    int channel = -1;
    for (int channel_index = 0; channel_index < device_data->analog.IO.channel_count; channel_index++) {
        if (device_data->analog.IO.channel_label[channel_index] == std::string("V+") || device_data->analog.IO.channel_label[channel_index] == std::string("p25V")) {
            channel = channel_index;
            break;
        }
    }
    if (channel != -1) {
        // find and set enable node
        int node = -1;
        for (int node_index = 0; node_index < device_data->analog.IO.node_count[channel]; node_index++) {
            if (device_data->analog.IO.node_name[channel][node_index] == std::string("Enable")) {
                node = node_index;
                break;
            }
        }
        if (node != -1) {
            if (FDwfAnalogIOChannelNodeSet(device_data->handle, channel, node, supplies_data.positive_state) == 0) {
                device.check_error(device_data);
            }
        }
        // find and set voltage node
        node = -1;
        for (int node_index = 0; node_index < device_data->analog.IO.node_count[channel]; node_index++) {
            if (device_data->analog.IO.node_name[channel][node_index] == std::string("Voltage")) {
                node = node_index;
                break;
            }
        }
        if (node != -1) {
            double voltage = tools.min(tools.max(supplies_data.positive_voltage, device_data->analog.IO.min_set_range[channel][node]), device_data->analog.IO.max_set_range[channel][node]);
            if (FDwfAnalogIOChannelNodeSet(device_data->handle, channel, node, voltage) == 0) {
                device.check_error(device_data);
            }
        }
        // find and set current node
        node = -1;
        for (int node_index = 0; node_index < device_data->analog.IO.node_count[channel]; node_index++) {
            if (device_data->analog.IO.node_name[channel][node_index] == std::string("Current")) {
                node = node_index;
                break;
            }
        }
        if (node != -1) {
            double current = tools.min(tools.max(supplies_data.positive_current, device_data->analog.IO.min_set_range[channel][node]), device_data->analog.IO.max_set_range[channel][node]);
            if (FDwfAnalogIOChannelNodeSet(device_data->handle, channel, node, current) == 0) {
                device.check_error(device_data);
            }
        }
    }

    //find the negative supply
    channel = -1;
    for (int channel_index = 0; channel_index < device_data->analog.IO.channel_count; channel_index++) {
        if (device_data->analog.IO.channel_label[channel_index] == std::string("V-") || device_data->analog.IO.channel_label[channel_index] == std::string("n25V")) {
            channel = channel_index;
            break;
        }
    }
    if (channel != -1) {
        // find and set enable node
        int node = -1;
        for (int node_index = 0; node_index < device_data->analog.IO.node_count[channel]; node_index++) {
            if (device_data->analog.IO.node_name[channel][node_index] == std::string("Enable")) {
                node = node_index;
                break;
            }
        }
        if (node != -1) {
            if (FDwfAnalogIOChannelNodeSet(device_data->handle, channel, node, supplies_data.negative_state) == 0) {
                device.check_error(device_data);
            }
        }
        // find and set voltage node
        node = -1;
        for (int node_index = 0; node_index < device_data->analog.IO.node_count[channel]; node_index++) {
            if (device_data->analog.IO.node_name[channel][node_index] == std::string("Voltage")) {
                node = node_index;
                break;
            }
        }
        if (node != -1) {
            double voltage = tools.min(tools.max(supplies_data.negative_voltage, device_data->analog.IO.min_set_range[channel][node]), device_data->analog.IO.max_set_range[channel][node]);
            if (FDwfAnalogIOChannelNodeSet(device_data->handle, channel, node, voltage) == 0) {
                device.check_error(device_data);
            }
        }
        // find and set current node
        node = -1;
        for (int node_index = 0; node_index < device_data->analog.IO.node_count[channel]; node_index++) {
            if (device_data->analog.IO.node_name[channel][node_index] == std::string("Current")) {
                node = node_index;
                break;
            }
        }
        if (node != -1) {
            double current = tools.min(tools.max(supplies_data.negative_current, device_data->analog.IO.min_set_range[channel][node]), device_data->analog.IO.max_set_range[channel][node]);
            if (FDwfAnalogIOChannelNodeSet(device_data->handle, channel, node, current) == 0) {
                device.check_error(device_data);
            }
        }
    }

    //find the digital/6V supply
    channel = -1;
    for (int channel_index = 0; channel_index < device_data->analog.IO.channel_count; channel_index++) {
        if (device_data->analog.IO.channel_label[channel_index] == std::string("VDD") || device_data->analog.IO.channel_label[channel_index] == std::string("p6V")) {
            channel = channel_index;
            break;
        }
    }
    if (channel != -1) {
        // find and set enable node
        int node = -1;
        for (int node_index = 0; node_index < device_data->analog.IO.node_count[channel]; node_index++) {
            if (device_data->analog.IO.node_name[channel][node_index] == std::string("Enable")) {
                node = node_index;
                break;
            }
        }
        if (node != -1) {
            if (FDwfAnalogIOChannelNodeSet(device_data->handle, channel, node, supplies_data.state) == 0) {
                device.check_error(device_data);
            }
        }
        // find and set voltage node
        node = -1;
        for (int node_index = 0; node_index < device_data->analog.IO.node_count[channel]; node_index++) {
            if (device_data->analog.IO.node_name[channel][node_index] == std::string("Voltage")) {
                node = node_index;
                break;
            }
        }
        if (node != -1) {
            double voltage = tools.min(tools.max(supplies_data.voltage, device_data->analog.IO.min_set_range[channel][node]), device_data->analog.IO.max_set_range[channel][node]);
            if (FDwfAnalogIOChannelNodeSet(device_data->handle, channel, node, voltage) == 0) {
                device.check_error(device_data);
            }
        }
        // find and set current node
        node = -1;
        for (int node_index = 0; node_index < device_data->analog.IO.node_count[channel]; node_index++) {
            if (device_data->analog.IO.node_name[channel][node_index] == std::string("Current")) {
                node = node_index;
                break;
            }
        }
        if (node != -1) {
            double current = tools.min(tools.max(supplies_data.current, device_data->analog.IO.min_set_range[channel][node]), device_data->analog.IO.max_set_range[channel][node]);
            if (FDwfAnalogIOChannelNodeSet(device_data->handle, channel, node, current) == 0) {
                device.check_error(device_data);
            }
        }
    }

    // turn all supplies on/off
    if (FDwfAnalogIOEnableSet(device_data->handle, supplies_data.master_state) == 0) {
        device.check_error(device_data);
    }
    return;
}

/* ----------------------------------------------------- */

void wf::Supplies::close(Device::Data *device_data) {
    /*
        reset the supplies
    */
    if (FDwfAnalogIOReset(device_data->handle) == 0) {
        device.check_error(device_data);
    }
    return;
}
