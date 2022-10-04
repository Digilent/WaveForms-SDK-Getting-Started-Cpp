/* DEVICE CONTROL FUNCTIONS: open, check_error, close, temperature */

/* include the header */
#include "device.h"

/* ----------------------------------------------------- */

wf::Device::Data* wf::Device::open(std::string device, int config) {
    /*
        open a specific device

        parameters: - device type: "" (first device), "Analog Discovery", "Analog Discovery 2",
                                   "Analog Discovery Studio", "Digital Discovery",
                                   "Analog Discovery Pro 3X50", "Analog Discovery Pro 5250"
                    - configuration

        returns:    - device data
    */

    Data *device_data = new Data();

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
        device_data->error.instrument = "device";
        device_data->error.function = "open";
        if (device_type == 0) {
            device_data->error.message = "There are no connected devices";
        }
        else {
            device_data->error.message = "There is no " + device + " connected";
        }
        throw device_data->error;
    }

    // this is the device handle - it will be used by all functions to "address" the connected device
    device_data->handle = 0;

    // connect to the first available device
    HDWF index = 0;
    while (device_data->handle == 0 && index < device_count) {
        FDwfDeviceConfigOpen(index, config, &device_data->handle);
        index++;    // increment the index and try again if the device is busy
    }

    // check connected device type
    device_data->name = "";
    if (device_data->handle != 0) {
        int device_id = 0;
        int device_rev = 0;
        FDwfEnumDeviceType(index - 1, &device_id, &device_rev);

        // decode device id
        for (std::map<std::string, int>::iterator pair = device_names.begin(); pair != device_names.end(); ++pair) {
            if (device_id == pair->second) {
                device_data->name = pair->first;
                break;
            }
        }
    }

    // check for errors
    // if the device handle is empty after a connection attempt
    if (device_data->handle == hdwfNone) {
        // check for errors
        int err_nr = 0; // variable for error number
        FDwfGetLastError(&err_nr);  // get error number

        // if there is an error
        if (err_nr != dwfercNoErc) {
            // check the error message
            check_error(device_data);
        }
    }

    // set global data
    get_info(device_data);
    return device_data;
}

/* ----------------------------------------------------- */

void wf::Device::check_error(Data *device_data, const char *caller, const char *file) {
    /*
        check for errors
    */
    char err_msg[512];  // variable for the error message
    FDwfGetLastErrorMsg(err_msg);  // get the error message
    device_data->error.message = err_msg;   // cast it to string
    if (device_data->error.message != "") {
        device_data->error.function = caller;
        device_data->error.instrument = file;
        // delete the extension
        size_t index = device_data->error.instrument.find('.');
        if (index != std::string::npos) {
            device_data->error.instrument = device_data->error.instrument.substr(0, index);
        }
        // delete the path
        device_data->error.instrument = std::string(device_data->error.instrument.rbegin(), device_data->error.instrument.rend());
        index = device_data->error.instrument.find('/');
        if (index != std::string::npos) {
            device_data->error.instrument = device_data->error.instrument.substr(0, index);
        }
        index = device_data->error.instrument.find('\\');
        if (index != std::string::npos) {
            device_data->error.instrument = device_data->error.instrument.substr(0, index);
        }
        device_data->error.instrument = std::string(device_data->error.instrument.rbegin(), device_data->error.instrument.rend());
        throw device_data->error;
    }
    return;
}

/* ----------------------------------------------------- */

void wf::Device::close(Data *device_data) {
    /*
        close a specific device
    */
    if (device_data->handle != 0) {
        FDwfDeviceClose(device_data->handle);
    }
    delete device_data;
    return;
}

/* ----------------------------------------------------- */

double wf::Device::temperature(Data *device_data) {
    /*
        return the board temperature
    */
    // find the system monitor
    int channel = -1;
    for (int channel_index = 0; channel_index < device_data->analog.IO.channel_count; channel_index++) {
        if (device_data->analog.IO.channel_label[channel_index] == "System") {
            channel = channel_index;
            break;
        }
    }
    if (channel < 0) {
        return 0;
    }

    // find the temperature node
    int node = -1;
    for (int node_index = 0; node_index < device_data->analog.IO.node_count[channel]; node_index++) {
        if (device_data->analog.IO.node_name[channel][node_index] == "Temp") {
            node = node_index;
            break;
        }
    }
    if (node < 0) {
        return 0;
    }

    // read the temperature
    if (FDwfAnalogIOStatus(device_data->handle) == 0) {
        check_error(device_data);
    }
    double temperature = 0;
    if (FDwfAnalogIOChannelNodeStatus(device_data->handle, channel, node, &temperature) == 0) {
        check_error(device_data);
    }
    return temperature;
}

/* ----------------------------------------------------- */

void wf::Device::get_info(Data* device_data) {
    /*
        get device information
    */

    // check WaveForms version
    char version[16];
    if (FDwfGetVersion(version) == 0) {
        check_error(device_data);
    }
    device_data->version = std::string(version);

    // analog input information
    // channel count
    int handle = device_data->handle;
    if (FDwfAnalogInChannelCount(handle, &device_data->analog.input.channel_count) == 0) {
        check_error(device_data);
    }
    // buffer size
    if (FDwfAnalogInBufferSizeInfo(handle, 0, &device_data->analog.input.max_buffer_size) == 0) {
        check_error(device_data);
    }
    // ADC resolution
    if (FDwfAnalogInBitsInfo(handle, &device_data->analog.input.max_resolution) == 0) {
        check_error(device_data);
    }
    // range information
    if (FDwfAnalogInChannelRangeInfo(handle, &device_data->analog.input.min_range, &device_data->analog.input.max_range, &device_data->analog.input.steps_range) == 0) {
        check_error(device_data);
    }
    // offset information
    if (FDwfAnalogInChannelOffsetInfo(handle, &device_data->analog.input.min_offset, &device_data->analog.input.max_offset, &device_data->analog.input.steps_offset) == 0) {
        check_error(device_data);
    }

    // analog output information
    // channel count
    if (FDwfAnalogOutCount(handle, &device_data->analog.output.channel_count) == 0) {
        check_error(device_data);
    }
    for (int channel_index = 0; channel_index < device_data->analog.output.channel_count; channel_index++) {
        // check node types and count
        int temp1;
        if (FDwfAnalogOutNodeInfo(handle, channel_index, &temp1) == 0) {
            check_error(device_data);
        }
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
        device_data->analog.output.node_count.insert(device_data->analog.output.node_count.end(), device_data->analog.output.node_type[channel_index].size());
        // buffer size
        std::vector<int> templist2;
        for (int node_index = 0; node_index < device_data->analog.output.node_count[channel_index]; node_index++) {
            if (FDwfAnalogOutNodeDataInfo(handle, channel_index, node_index, 0, &temp1) == 0) {
                check_error(device_data);
            }
            templist2.insert(templist2.end(), temp1);
        }
        device_data->analog.output.max_buffer_size.insert(device_data->analog.output.max_buffer_size.end(), templist2);
        // amplitude information
        std::vector<double> templist3, templist4;
        double temp3, temp4;
        for (int node_index = 0; node_index < device_data->analog.output.node_count[channel_index]; node_index++) {
            if (FDwfAnalogOutNodeAmplitudeInfo(handle, channel_index, node_index, &temp3, &temp4) == 0) {
                check_error(device_data);
            }
            templist3.insert(templist3.end(), temp3);
            templist4.insert(templist4.end(), temp4);
        }
        device_data->analog.output.min_amplitude.insert(device_data->analog.output.min_amplitude.end(), templist3);
        device_data->analog.output.max_amplitude.insert(device_data->analog.output.max_amplitude.end(), templist4);
        // offset information
        templist3.clear();
        templist4.clear();
        for (int node_index = 0; node_index < device_data->analog.output.node_count[channel_index]; node_index++) {
            if (FDwfAnalogOutNodeOffsetInfo(handle, channel_index, node_index, &temp3, &temp4) == 0) {
                check_error(device_data);
            }
            templist3.insert(templist3.end(), temp3);
            templist4.insert(templist4.end(), temp4);
        }
        device_data->analog.output.min_offset.insert(device_data->analog.output.min_offset.end(), templist3);
        device_data->analog.output.max_offset.insert(device_data->analog.output.max_offset.end(), templist4);
        // frequency information
        templist3.clear();
        templist4.clear();
        for (int node_index = 0; node_index < device_data->analog.output.node_count[channel_index]; node_index++) {
            if (FDwfAnalogOutNodeFrequencyInfo(handle, channel_index, node_index, &temp3, &temp4) == 0) {
                check_error(device_data);
            }
            templist3.insert(templist3.end(), temp3);
            templist4.insert(templist4.end(), temp4);
        }
        device_data->analog.output.min_frequency.insert(device_data->analog.output.min_frequency.end(), templist3);
        device_data->analog.output.max_frequency.insert(device_data->analog.output.max_frequency.end(), templist4);
    }

    // analog IO information
    // channel count
    if (FDwfAnalogIOChannelCount(handle, &device_data->analog.IO.channel_count) == 0) {
        check_error(device_data);
    }
    for (int channel_index = 0; channel_index < device_data->analog.IO.channel_count; channel_index++) {
        // channel names and labels
        char temp1[256];
        char temp2[256];
        if (FDwfAnalogIOChannelName(handle, channel_index, temp1, temp2) == 0) {
            check_error(device_data);
        }
        device_data->analog.IO.channel_name.insert(device_data->analog.IO.channel_name.end(), std::string(temp1));
        device_data->analog.IO.channel_label.insert(device_data->analog.IO.channel_label.end(), std::string(temp2));
        // node count
        int temp3;
        if (FDwfAnalogIOChannelInfo(handle, channel_index, &temp3) == 0) {
            check_error(device_data);
        }
        device_data->analog.IO.node_count.insert(device_data->analog.IO.node_count.end(), temp3);
        // node names and units
        std::vector<std::string> templist1, templist2;
        for (int node_index = 0; node_index < device_data->analog.IO.node_count[channel_index]; node_index++) {
            if (FDwfAnalogIOChannelNodeName(handle, channel_index, node_index, temp1, temp2) == 0) {
                check_error(device_data);
            }
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
            if (FDwfAnalogIOChannelNodeSetInfo(handle, channel_index, node_index, &temp4, &temp5, &temp3) == 0) {
                check_error(device_data);
            }
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
            if (FDwfAnalogIOChannelNodeStatusInfo(handle, channel_index, node_index, &temp4, &temp5, &temp3) == 0) {
                check_error(device_data);
            }
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
    if (FDwfDigitalInBitsInfo(handle, &device_data->digital.input.channel_count) == 0) {
        check_error(device_data);
    }
    // buffer size
    if (FDwfDigitalInBufferSizeInfo(handle, &device_data->digital.input.max_buffer_size) == 0) {
        check_error(device_data);
    }

    // digital output information
    // channel count
    if (FDwfDigitalOutCount(handle, &device_data->digital.output.channel_count) == 0) {
        check_error(device_data);
    }
    // buffer size
    unsigned int temp;
    if (FDwfDigitalOutDataInfo(handle, 0, &temp) == 0) {
        check_error(device_data);
    }
    device_data->digital.output.max_buffer_size = (int)temp;

    return;
}
