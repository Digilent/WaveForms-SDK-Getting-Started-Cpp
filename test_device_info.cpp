#include "WF_SDK/WF_SDK.h"  // include all classes and functions
#include <fstream>          // needed for input/output
#include <string>           // needed for string operations

/* ----------------------------------------------------- */

int main(void) {
    // connect to the device
    Device::Data device_data = device.open();

    // check for connection errors
    device.check_error(device_data);

    /* ----------------------------------------------------- */

    // set output file name
    std::string filename = device_data.name + ".log";
    std::ofstream out_file(filename);

    // print device information
    out_file << "WaveForms version: " << device_data.version << std::endl;

    // print device name
    out_file << "Device name: " << device_data.name << std::endl;

    // print analog input information
    out_file << "Analog input information:" << std::endl;
    out_file << "\tchannels: " << std::to_string(device_data.analog.input.channel_count) << std::endl;
    out_file << "\tmaximum buffer size: " << std::to_string(device_data.analog.input.max_buffer_size) << std::endl;
    out_file << "\tADC resolution: " << std::to_string(device_data.analog.input.max_resolution) << " bits" << std::endl;
    out_file << "\trange settable from " << std::to_string(device_data.analog.input.min_range) << "V to " <<
        std::to_string(device_data.analog.input.max_range) << "V in " <<
        std::to_string(device_data.analog.input.steps_range) << " steps" << std::endl;
    out_file << "\toffset settable from " << std::to_string(device_data.analog.input.min_offset) << "V to " <<
        std::to_string(device_data.analog.input.max_offset) << "V in " <<
        std::to_string(device_data.analog.input.steps_offset) << " steps\n" << std::endl;

    // print analog output information
    out_file << "Analog output information:" << std::endl;
    for (int channel_index = 0; channel_index < device_data.analog.output.channel_count; channel_index++) {
        out_file << "\tchannel " << std::to_string(channel_index) << ":" << std::endl;
        for (int node_index = 0; node_index < device_data.analog.output.node_count[channel_index]; node_index++) {
            out_file << "\t\tnode " << std::to_string(node_index) << ":" << std::endl;
            out_file << "\t\t\tnode type: " << device_data.analog.output.node_type[channel_index][node_index] << std::endl;
            out_file << "\t\t\tmaximum buffer size: " << std::to_string(device_data.analog.output.max_buffer_size[channel_index][node_index]) << std::endl;
            out_file << "\t\t\tamplitude settable from: " << std::to_string(device_data.analog.output.min_amplitude[channel_index][node_index]) << "V to " <<
                std::to_string(device_data.analog.output.max_amplitude[channel_index][node_index]) << "V" << std::endl;
            out_file << "\t\t\toffset settable from: " << std::to_string(device_data.analog.output.min_offset[channel_index][node_index]) << "V to " <<
                std::to_string(device_data.analog.output.max_offset[channel_index][node_index]) << "V" << std::endl;
            out_file << "\t\t\tfrequency settable from: " << std::to_string(device_data.analog.output.min_frequency[channel_index][node_index]) << "Hz to " <<
                std::to_string(device_data.analog.output.max_frequency[channel_index][node_index]) << "Hz\n" << std::endl;
        }
    }

    // print analog IO information
    out_file << "Analog IO information:" << std::endl;
    for (int channel_index = 0; channel_index < device_data.analog.IO.channel_count; channel_index++) {
        out_file << "\tchannel " << std::to_string(channel_index) << ":" << std::endl;
        out_file << "\t\tchannel name: " << device_data.analog.IO.channel_name[channel_index] << std::endl;
        out_file << "\t\tchannel label: " << device_data.analog.IO.channel_label[channel_index] << std::endl;
        for (int node_index = 0; node_index < device_data.analog.IO.node_count[channel_index]; node_index++) {
            out_file << "\t\tnode " << std::to_string(node_index) << ":" << std::endl;
            out_file << "\t\t\tnode name: " << device_data.analog.IO.node_name[channel_index][node_index] << std::endl;
            out_file << "\t\t\tunit of measurement: " << device_data.analog.IO.node_unit[channel_index][node_index] << std::endl;
            out_file << "\t\t\tsettable from: " << std::to_string(device_data.analog.IO.min_set_range[channel_index][node_index]) << " to " <<
                std::to_string(device_data.analog.IO.max_set_range[channel_index][node_index]) << " in " <<
                std::to_string(device_data.analog.IO.set_steps[channel_index][node_index]) << " steps" << std::endl;
            out_file << "\t\t\treadable between: " << std::to_string(device_data.analog.IO.min_read_range[channel_index][node_index]) << " to " <<
                std::to_string(device_data.analog.IO.max_read_range[channel_index][node_index]) << " in " <<
                std::to_string(device_data.analog.IO.read_steps[channel_index][node_index]) << " steps\n" << std::endl;
        }
    }

    // print digital input information
    out_file << "Digital input information:" << std::endl;
    out_file << "\tchannels: " << std::to_string(device_data.digital.input.channel_count) << std::endl;
    out_file << "\tmaximum buffer size: " << std::to_string(device_data.digital.input.max_buffer_size) << std::endl << std::endl;

    // print digital output information
    out_file << "Digital output information:" << std::endl;
    out_file << "\tchannels: " << std::to_string(device_data.digital.output.channel_count) << std::endl;
    out_file << "\tmaximum buffer size: " << std::to_string(device_data.digital.output.max_buffer_size) << std::endl;


    /* ----------------------------------------------------- */

    // close the connection
    device.close(device_data);
    return 0;
}
