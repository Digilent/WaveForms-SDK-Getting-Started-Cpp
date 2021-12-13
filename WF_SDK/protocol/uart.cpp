/* PROTOCOL: UART CONTROL FUNCTIONS: open, read, write, close */

/* include the header */
#include "uart.h"

/* ----------------------------------------------------- */

void UART::open(HDWF device_handle, int rx, int tx, int baud_rate, bool parity, int data_bits, int stop_bits) {
    /*
        initializes UART communication
        
        parameters: - device handle
                    - rx (DIO line used to receive data)
                    - tx (DIO line used to send data)
                    - baud_rate (communication speed, default is 9600 bits/s)
                    - parity possible: None (default), True means even, False means odd
                    - data_bits (default is 8)
                    - stop_bits (default is 1)
    */
    // set baud rate
    FDwfDigitalUartRateSet(device_handle, double(baud_rate));

    // set communication channels
    FDwfDigitalUartTxSet(device_handle, tx);
    FDwfDigitalUartRxSet(device_handle, rx);

    // set data bit count
    FDwfDigitalUartBitsSet(device_handle, data_bits);

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
    FDwfDigitalUartParitySet(device_handle, par);

    // set stop bit count
    FDwfDigitalUartStopSet(device_handle, double(stop_bits));

    // initialize channels with idle levels

    // dummy read
    char* dummy_buffer1 = nullptr;
    int dummy_buffer2 = 0;
    int dummy_parity_flag = 0;
    FDwfDigitalUartRx(device_handle, dummy_buffer1, 0, &dummy_buffer2, &dummy_parity_flag);

    // dummy write
    FDwfDigitalUartTx(device_handle, dummy_buffer1, 0);
    return;
}

/* ----------------------------------------------------- */

uart_data UART::read(HDWF device_handle) {
    /*
        receives data from UART
        
        parameters: - device handle

        return:     - integer list containing the received bytes
                    - error message or empty string
    */
    // variable to store results
    uart_data out_data;
    vector<unsigned char> rx_data;

    // create empty string buffer
    char data[8193];

    // character counter
    int count = 0;

    // parity flag
    int parity_flag = 0;

    // read up to 8k characters
    FDwfDigitalUartRx(device_handle, data, sizeof(data) / sizeof(data[0]) - 1, &count, &parity_flag);

    // append current data chunks
    int index = 0;
    for (index = 0; index < count; index++) {
        rx_data[index] = (unsigned char)(data[index]);
    }

    // ensure data integrity
    while (count > 0) {
        // reset character counter
        count = 0;

        // parity flag
        parity_flag = 0;

        // read up to 8k characters
        FDwfDigitalUartRx(device_handle, data, sizeof(data) / sizeof(data[0]) - 1, &count, &parity_flag);

        // append current data chunks
        for ( ; index < count; index++) {
            rx_data[index] = (unsigned char)(data[index]);
        }

        // check for not acknowledged
        if (parity_flag < 0) {
            out_data.error = "Buffer overflow";
        }
        else if (parity_flag > 0) {
            out_data.error = "Parity error: index " + to_string(parity_flag);
        }
    }

    out_data.data = rx_data;
    return out_data;
}

/* ----------------------------------------------------- */

void UART::write(HDWF device_handle, string data) {
    /*
        send data through UART
        
        parameters: - data of type string
    */
    // encode the string into a C string
    vector<char> buffer(data.size() + 1);
    strcpy(buffer.data(), data.c_str());

    // send text, trim zero ending
    FDwfDigitalUartTx(device_handle, buffer.data(), buffer.size() - 1);
    return;
}

/* ----------------------------------------------------- */

void UART::write(HDWF device_handle, vector<unsigned char> data) {
    /*
        send data through UART
        
        parameters: - data of type array of integers
    */
    // encode the array into a C string
    vector<char> buffer(data.size());
    for (int index = 0; index < data.size(); index++) {
        buffer[index] = char(data[index]);
    } 

    // send text, trim zero ending
    FDwfDigitalUartTx(device_handle, buffer.data(), buffer.size() - 1);
    return;
}

/* ----------------------------------------------------- */

void UART::close(HDWF device_handle) {
    /*
        reset the uart interface
    */
    FDwfDigitalUartReset(device_handle);
    return;
}
