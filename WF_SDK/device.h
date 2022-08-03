/* DEVICE CONTROL FUNCTIONS: open, check_error, close, temperature */

/* include the necessary libraries */
#include <string>
#include <iterator>
#include <map>
#include <iostream>
#include <vector>
#include "dwf.h"

#ifndef WF_DEVICE
#define WF_DEVICE

class Device {
    // private class definitions
private:
    class State {
    public:
        bool connected = false;
        bool disconnected = true;
        std::string error = "";
    };

    // public class definitions
public:
    class Data {
    private:
        class analog_data {
        private:
            class input_data {
            public:
                int channel_count = 0;
                int max_buffer_size = 0;
                int max_resolution = 0;
                double min_range = 0;
                double max_range = 0;
                double steps_range = 0;
                double min_offset = 0;
                double max_offset = 0;
                double steps_offset = 0;
                input_data& operator=(const input_data& data) {
                    if (this != &data) {
                        channel_count = data.channel_count;
                        max_buffer_size = data.max_buffer_size;
                        max_resolution = data.max_resolution;
                        min_range = data.min_range;
                        max_range = data.max_range;
                        steps_range = data.steps_range;
                        min_offset = data.min_offset;
                        max_offset = data.max_offset;
                        steps_offset = data.steps_offset;
                    }
                    return *this;
                }
            };
            class output_data {
            public:
                int channel_count = 0;
                std::vector<int> node_count;
                std::vector<std::vector<std::string>> node_type;
                std::vector<std::vector<int>> max_buffer_size;
                std::vector<std::vector<double>> min_amplitude;
                std::vector<std::vector<double>> max_amplitude;
                std::vector<std::vector<double>> min_offset;
                std::vector<std::vector<double>> max_offset;
                std::vector<std::vector<double>> min_frequency;
                std::vector<std::vector<double>> max_frequency;
                output_data& operator=(const output_data& data) {
                    if (this != &data) {
                        channel_count = data.channel_count;
                        node_count = data.node_count;
                        node_type = data.node_type;
                        max_buffer_size = data.max_buffer_size;
                        min_amplitude = data.min_amplitude;
                        max_amplitude = data.max_amplitude;
                        min_offset = data.min_offset;
                        max_offset = data.max_offset;
                        min_frequency = data.min_frequency;
                        max_frequency = data.max_frequency;
                    }
                    return *this;
                }
            };
            class IO_data {
            public:
                int channel_count = 0;
                std::vector<int> node_count;
                std::vector<std::string> channel_name;
                std::vector<std::string> channel_label;
                std::vector<std::vector<std::string>> node_name;
                std::vector<std::vector<std::string>> node_unit;
                std::vector<std::vector<double>> min_set_range;
                std::vector<std::vector<double>> max_set_range;
                std::vector<std::vector<double>> min_read_range;
                std::vector<std::vector<double>> max_read_range;
                std::vector<std::vector<int>> set_steps;
                std::vector<std::vector<int>> read_steps;
                IO_data& operator=(const IO_data& data) {
                    if (this != &data) {
                        channel_count = data.channel_count;
                        node_count = data.node_count;
                        channel_name = data.channel_name;
                        channel_label = data.channel_label;
                        node_name = data.node_name;
                        node_unit = data.node_unit;
                        min_set_range = data.min_set_range;
                        max_set_range = data.max_set_range;
                        min_read_range = data.min_read_range;
                        max_read_range = data.max_read_range;
                        set_steps = data.set_steps;
                        read_steps = data.read_steps;
                    }
                    return *this;
                }

            };

        public:
            input_data input;
            output_data output;
            IO_data IO;
            analog_data& operator=(const analog_data& data) {
                if (this != &data) {
                    input = data.input;
                    output = data.output;
                    IO = data.IO;
                }
                return *this;
            }
        };

        class digital_data {
        private:
            class input_data {
            public:
                int channel_count = 0;
                int max_buffer_size = 0;
                input_data& operator=(const input_data& data) {
                    if (this != &data) {
                        channel_count = data.channel_count;
                        max_buffer_size = data.max_buffer_size;
                    }
                    return *this;
                }
            };
            class output_data {
            public:
                int channel_count = 0;
                int max_buffer_size = 0;
                output_data& operator=(const output_data& data) {
                    if (this != &data) {
                        channel_count = data.channel_count;
                        max_buffer_size = data.max_buffer_size;
                    }
                    return *this;
                }
            };

        public:
            input_data input;
            output_data output;
            digital_data& operator=(const digital_data& data) {
                if (this != &data) {
                    input = data.input;
                    output = data.output;
                }
                return *this;
            }
        };

    public:
        HDWF handle = 0;
        std::string name = "";
        std::string version = "";
        analog_data analog;
        digital_data digital;

        Data& operator=(const Data& data) {
            if (this != &data) {
                handle = data.handle;
                name = data.name;
                version = data.version;
                analog = data.analog;
                digital = data.digital;
            }
            return *this;
        }
    };

    // private function definitions
private:
    void get_info(Data* device_data);

    // public function definitions
public:
    State state;
    Data open(std::string device = "", int config = 0);
    void check_error(Data device_data);
    void close(Data device_data);
    double temperature(Data device_data);
} device;

#endif
