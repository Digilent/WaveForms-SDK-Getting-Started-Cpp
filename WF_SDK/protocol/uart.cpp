/* PROTOCOL: UART CONTROL FUNCTIONS: open, read, write, close */

/* include the header */
#include "uart.h"

/* ----------------------------------------------------- */

void wf::UART::open(Device::Data *device_data, int rx, int tx, int baud_rate, bool parity, int data_bits, int stop_bits) {
    /*
        initializes UART communication
        
        parameters: - device data
                    - rx (DIO line used to receive data)
                    - tx (DIO line used to send data)
                    - baud_rate (communication speed, default is 9600 bits/s)
                    - parity possible: None (default), True means even, False means odd
                    - data_bits (default is 8)
                    - stop_bits (default is 1)
    */
    // set baud rate
    if (FDwfDigitalUartRateSet(device_data->handle, double(baud_rate)) == 0) {
        device.check_error(device_data);
    }

    // set communication channels
    if (FDwfDigitalUartTxSet(device_data->handle, tx) == 0) {
        device.check_error(device_data);
    }
    if (FDwfDigitalUartRxSet(device_data->handle, rx) == 0) {
        device.check_error(device_data);
    }

    // set data bit count
    if (FDwfDigitalUartBitsSet(device_data->handle, data_bits) == 0) {
        device.check_error(device_data);
    }

    // set parity bit requirements
    int par = 0;
    if (parity == true) {
        par = 2;
    }
    else if (parity == false) {
        par = 1;
    }
    else {
        par = 0;
    }
    if (FDwfDigitalUartParitySet(device_data->handle, par) == 0) {
        device.check_error(device_data);
    }

    // set stop bit count
    if (FDwfDigitalUartStopSet(device_data->handle, double(stop_bits)) == 0) {
        device.check_error(device_data);
    }

    // initialize channels with idle levels

    // dummy read
    char* dummy_buffer1 = nullptr;
    int dummy_buffer2 = 0;
    int dummy_parity_flag = 0;
    if (FDwfDigitalUartRx(device_data->handle, dummy_buffer1, 0, &dummy_buffer2, &dummy_parity_flag) == 0) {
        device.check_error(device_data);
    }

    // dummy write
    if (FDwfDigitalUartTx(device_data->handle, dummy_buffer1, 0) == 0) {
        device.check_error(device_data);
    }
    return;
}

/* ----------------------------------------------------- */

std::vector<unsigned char> wf::UART::read(Device::Data *device_data) {
    /*
        receives data from UART
        
        parameters: - device data
                    - pointer to error string

        return:     - integer list containing the received bytes
                    - error message or empty string
    */
    // variable to store results
    std::vector<char> rx_data(device_data->digital.input.max_buffer_size);
    std::vector<unsigned char> data;

    // character counter
    int count = 0;

    // parity flag
    int parity_flag = 0;

    // read up to 8k characters
    if (FDwfDigitalUartRx(device_data->handle, rx_data.data(), rx_data.size() - 1, &count, &parity_flag) == 0) {
        device.check_error(device_data);
    }

    // append current data chunks
    int index = 0;
    for (index = 0; index < count; index++) {
        data.insert(data.end(), (unsigned char)(rx_data[index]));
    }

    // ensure data integrity
    while (count > 0) {
        // reset character counter
        count = 0;

        // parity flag
        parity_flag = 0;

        // read up to 8k characters
        if (FDwfDigitalUartRx(device_data->handle, rx_data.data(), rx_data.size() - 1, &count, &parity_flag) == 0) {
            device.check_error(device_data);
        }

        // append current data chunks
        for ( ; index < count; index++) {
            data.insert(data.end(), (unsigned char)(rx_data[index]));
        }

        // check for not acknowledged
        if (parity_flag < 0) {
            device_data->warning.instrument = "protocol/uart";
            device_data->warning.function = "read";
            device_data->warning.message = "Buffer overflow";
            throw device_data->warning;
        }
        else if (parity_flag > 0) {
            device_data->warning.instrument = "protocol/uart";
            device_data->warning.function = "read";
            device_data->warning.message = "Parity error: index " + std::to_string(parity_flag);
            throw device_data->warning;
        }
    }
    return data;
}

/* ----------------------------------------------------- */

void wf::UART::write(Device::Data *device_data, std::string data) {
    /*
        send data through UART
        
        parameters: - data of type string
    */
    //  create buffer to write
    std::vector<unsigned char> buffer(data.size() + 1);
    for (int index = 0; index < buffer.size(); index++) {
        buffer[index] = (unsigned char)(data[index]);
    }
    write(device_data, buffer);
    return;
}

/* ----------------------------------------------------- */

void wf::UART::write(Device::Data *device_data, std::vector<unsigned char> data) {
    /*
        send data through UART
        
        parameters: - data of type array of integers
    */
    // encode the string into a C string
    std::vector<char> buffer(data.size() + 1);
    for (int index = 0; index < buffer.size(); index++) {
        buffer[index] = (char)(data[index]);
    }
    // send text, trim zero ending
    if (FDwfDigitalUartTx(device_data->handle, buffer.data(), buffer.size() - 1) == 0) {
        device.check_error(device_data);
    }
    return;
}

/* ----------------------------------------------------- */

void wf::UART::close(Device::Data *device_data) {
    /*
        reset the uart interface
    */
    if (FDwfDigitalUartReset(device_data->handle) == 0) {
        device.check_error(device_data);
    }
    return;
}
