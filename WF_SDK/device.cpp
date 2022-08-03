/* DEVICE CONTROL FUNCTIONS: open, check_error, close, temperature */

/* include the header */
#include "device.h"

/* ----------------------------------------------------- */

Device::Data Device::open(std::string device, int config) {
    /*
        open a specific device

        parameters: - device type: "" (first device), "Analog Discovery", "Analog Discovery 2",
                                   "Analog Discovery Studio", "Digital Discovery",
                                   "Analog Discovery Pro 3X50", "Analog Discovery Pro 5250"
                    - configuration

        returns:    - device data
    */

    Data device_data;

    std::map<std::string, int> device_names;
    device_names["Analog Discovery"] = devidDiscovery;
    device_names["Analog Discovery 2"] = devidDiscovery2;
    device_names["Analog Discovery Studio"] = devidDiscovery2;
    device_names["Digital Discovery"] = devidDDiscovery;
    device_names["Analog Discovery Pro 3X50"] = devidADP3X50;
    device_names["Analog Discovery Pro 5250"] = devidADP5250;

    // decode device names
    ENUMFILTER device_type = enumfilterAll;
    for (std::map<std::string, int>::iterator pair = device_names.begin(); pair != device_names.end(); ++pair) {
        if (device == pair->first) {
            device_type = pair->second;
            break;
        }
    }

    // count devices
    int device_count = 0;
    FDwfEnum(device_type, &device_count);

    // check for connected devices
    if (device_count <= 0) {
        if (device_type == 0) {
            std::cout << "Error: There are no connected devices" << std::endl;
        }
        else {
            std::cout << "Error: There is no " << device << " connected" << std::endl;
        }
        throw std::exception();
    }

    // this is the device handle - it will be used by all functions to "address" the connected device
    device_data.handle = 0;

    // connect to the first available device
    HDWF index = 0;
    while (device_data.handle == 0 && index < device_count) {
        FDwfDeviceConfigOpen(index, config, &device_data.handle);
        index++;    // increment the index and try again if the device is busy
    }

    // check connected device type
    device_data.name = "";
    if (device_data.handle != 0) {
        int device_id = 0;
        int device_rev = 0;
        FDwfEnumDeviceType(index - 1, &device_id, &device_rev);

        // decode device id
        for (std::map<std::string, int>::iterator pair = device_names.begin(); pair != device_names.end(); ++pair) {
            if (device_id == pair->second) {
                device_data.name = pair->first;
                break;
            }
        }
    }

    // set global data
    get_info(&device_data);
    state.connected = true;
    state.disconnected = false;
    return device_data;
}

/* ----------------------------------------------------- */

void Device::check_error(Data device_data) {
    /*
        check for connection errors
    */
    // if the device handle is empty after a connection attempt
    if (device_data.handle == hdwfNone) {
        // check for errors
        int err_nr = 0; // variable for error number
        FDwfGetLastError(&err_nr);  // get error number

        // if there is an error
        if (err_nr != dwfercNoErc) {
            // display it and quit
            char err_msg[512];  // variable for the error message
            FDwfGetLastErrorMsg(err_msg);  // get the error message
            std::cout << "Error: " << err_msg << std::endl;   // display error message
            state.error = err_msg;
            state.connected = false;
            state.disconnected = true;
            throw std::exception();  // exit the program
        }
    }
    return;
}

/* ----------------------------------------------------- */

void Device::close(Data device_data) {
    /*
        close a specific device
    */
    FDwfDeviceClose(device_data.handle);
    device_data.handle = 0;
    device_data.name = "";
    state.connected = false;
    state.disconnected = true;
    return;
}

/* ----------------------------------------------------- */

double Device::temperature(Data device_data) {
    /*
        return the board temperature
    */
    // find the system monitor
    int channel = -1;
    for (int channel_index = 0; channel_index < device_data.analog.IO.channel_count; channel_index++) {
        if (device_data.analog.IO.channel_label[channel_index] == "System") {
            channel = channel_index;
            break;
        }
    }
    if (channel < 0) {
        return 0;
    }

    // find the temperature node
    int node = -1;
    for (int node_index = 0; node_index < device_data.analog.IO.node_count[channel]; node_index++) {
        if (device_data.analog.IO.node_name[channel][node_index] == "Temp") {
            node = node_index;
            break;
        }
    }
    if (node < 0) {
        return 0;
    }

    // read the temperature
    FDwfAnalogIOStatus(device_data.handle);
    double temperature = 0;
    FDwfAnalogIOChannelNodeStatus(device_data.handle, channel, node, &temperature);
    return temperature;
}

/* ----------------------------------------------------- */

void Device::get_info(Data* device_data) {
    /*
        get device information
    */

    // check WaveForms version
    char version[16];
    FDwfGetVersion(version);
    device_data->version = std::string(version);

    // analog input information
    // channel count
    int handle = device_data->handle;
    FDwfAnalogInChannelCount(handle, &device_data->analog.input.channel_count);
    // buffer size
    FDwfAnalogInBufferSizeInfo(handle, 0, &device_data->analog.input.max_buffer_size);
    // ADC resolution
    FDwfAnalogInBitsInfo(handle, &device_data->analog.input.max_resolution);
    // range information
    FDwfAnalogInChannelRangeInfo(handle, &device_data->analog.input.min_range, &device_data->analog.input.max_range, &device_data->analog.input.steps_range);
    // offset information
    FDwfAnalogInChannelOffsetInfo(handle, &device_data->analog.input.min_offset, &device_data->analog.input.max_offset, &device_data->analog.input.steps_offset);

    // analog output information
    // channel count
    FDwfAnalogOutCount(handle, &device_data->analog.output.channel_count);
    for (int channel_index = 0; channel_index < device_data->analog.output.channel_count; channel_index++) {
        // check node types and count
        int temp1;
        FDwfAnalogOutNodeInfo(handle, channel_index, &temp1);
        std::vector<std::string> templist1;
        for (int node_index = 0; node_index < 3; node_index++) {
            if (((1 << node_index) & temp1) == 0) {
                continue;
            }
            else if (node_index == AnalogOutNodeCarrier) {
                templist1.insert(templist1.end(), std::string("carrier"));
            }
            else if (node_index == AnalogOutNodeFM) {
                templist1.insert(templist1.end(), std::string("FM"));
            }
            else if (node_index == AnalogOutNodeAM) {
                templist1.insert(templist1.end(), std::string("AM"));
            }
        }
        device_data->analog.output.node_type.insert(device_data->analog.output.node_type.end(), templist1);
        device_data->analog.output.node_count.insert(device_data->analog.output.node_count.end(), device_data->analog.output.node_type.size());
        // buffer size
        std::vector<int> templist2;
        for (int node_index = 0; node_index < device_data->analog.output.node_count[channel_index]; node_index++) {
            FDwfAnalogOutNodeDataInfo(handle, channel_index, node_index, 0, &temp1);
            templist2.insert(templist2.end(), temp1);
        }
        device_data->analog.output.max_buffer_size.insert(device_data->analog.output.max_buffer_size.end(), templist2);
        // amplitude information
        std::vector<double> templist3, templist4;
        double temp3, temp4;
        for (int node_index = 0; node_index < device_data->analog.output.node_count[channel_index]; node_index++) {
            FDwfAnalogOutNodeAmplitudeInfo(handle, channel_index, node_index, &temp3, &temp4);
            templist3.insert(templist3.end(), temp3);
            templist4.insert(templist4.end(), temp4);
        }
        device_data->analog.output.min_amplitude.insert(device_data->analog.output.min_amplitude.end(), templist3);
        device_data->analog.output.max_amplitude.insert(device_data->analog.output.max_amplitude.end(), templist4);
        // offset information
        templist3.clear();
        templist4.clear();
        for (int node_index = 0; node_index < device_data->analog.output.node_count[channel_index]; node_index++) {
            FDwfAnalogOutNodeOffsetInfo(handle, channel_index, node_index, &temp3, &temp4);
            templist3.insert(templist3.end(), temp3);
            templist4.insert(templist4.end(), temp4);
        }
        device_data->analog.output.min_offset.insert(device_data->analog.output.min_offset.end(), templist3);
        device_data->analog.output.max_offset.insert(device_data->analog.output.max_offset.end(), templist4);
        // frequency information
        templist3.clear();
        templist4.clear();
        for (int node_index = 0; node_index < device_data->analog.output.node_count[channel_index]; node_index++) {
            FDwfAnalogOutNodeFrequencyInfo(handle, channel_index, node_index, &temp3, &temp4);
            templist3.insert(templist3.end(), temp3);
            templist4.insert(templist4.end(), temp4);
        }
        device_data->analog.output.min_frequency.insert(device_data->analog.output.min_frequency.end(), templist3);
        device_data->analog.output.max_frequency.insert(device_data->analog.output.max_frequency.end(), templist4);
    }

    // analog IO information
    // channel count
    FDwfAnalogIOChannelCount(handle, &device_data->analog.IO.channel_count);
    for (int channel_index = 0; channel_index < device_data->analog.IO.channel_count; channel_index++) {
        // channel names and labels
        char temp1[256];
        char temp2[256];
        FDwfAnalogIOChannelName(handle, channel_index, temp1, temp2);
        device_data->analog.IO.channel_name.insert(device_data->analog.IO.channel_name.end(), std::string(temp1));
        device_data->analog.IO.channel_label.insert(device_data->analog.IO.channel_label.end(), std::string(temp2));
        // node count
        int temp3;
        FDwfAnalogIOChannelInfo(handle, channel_index, &temp3);
        device_data->analog.IO.node_count.insert(device_data->analog.IO.node_count.end(), temp3);
        // node names and units
        std::vector<std::string> templist1, templist2;
        for (int node_index = 0; node_index < device_data->analog.IO.node_count[channel_index]; node_index++) {
            FDwfAnalogIOChannelNodeName(handle, channel_index, node_index, temp1, temp2);
            templist1.insert(templist1.end(), temp1);
            templist2.insert(templist2.end(), temp2);
        }
        device_data->analog.IO.node_name.insert(device_data->analog.IO.node_name.end(), templist1);
        device_data->analog.IO.node_unit.insert(device_data->analog.IO.node_unit.end(), templist2);
        // node write info
        double temp4, temp5;
        std::vector<int> templist3;
        std::vector<double> templist4, templist5;
        for (int node_index = 0; node_index < device_data->analog.IO.node_count[channel_index]; node_index++) {
            FDwfAnalogIOChannelNodeSetInfo(handle, channel_index, node_index, &temp4, &temp5, &temp3);
            templist3.insert(templist3.end(), temp3);
            templist4.insert(templist4.end(), temp4);
            templist5.insert(templist5.end(), temp5);
        }
        device_data->analog.IO.min_set_range.insert(device_data->analog.IO.min_set_range.end(), templist4);
        device_data->analog.IO.max_set_range.insert(device_data->analog.IO.max_set_range.end(), templist5);
        device_data->analog.IO.set_steps.insert(device_data->analog.IO.set_steps.end(), templist3);
        // node read info
        templist3.clear();
        templist4.clear();
        templist5.clear();
        for (int node_index = 0; node_index < device_data->analog.IO.node_count[channel_index]; node_index++) {
            FDwfAnalogIOChannelNodeStatusInfo(handle, channel_index, node_index, &temp4, &temp5, &temp3);
            templist3.insert(templist3.end(), temp3);
            templist4.insert(templist4.end(), temp4);
            templist5.insert(templist5.end(), temp5);
        }
        device_data->analog.IO.min_read_range.insert(device_data->analog.IO.min_read_range.end(), templist4);
        device_data->analog.IO.max_read_range.insert(device_data->analog.IO.max_read_range.end(), templist5);
        device_data->analog.IO.read_steps.insert(device_data->analog.IO.read_steps.end(), templist3);
    }

    // digital input information
    // channel count
    FDwfDigitalInBitsInfo(handle, &device_data->digital.input.channel_count);
    // buffer size
    FDwfDigitalInBufferSizeInfo(handle, &device_data->digital.input.max_buffer_size);

    // digital output information
    // channel count
    FDwfDigitalOutCount(handle, &device_data->digital.output.channel_count);
    // buffer size
    unsigned int temp;
    FDwfDigitalOutDataInfo(handle, 0, &temp);
    device_data->digital.output.max_buffer_size = (int)temp;

    return;
}
