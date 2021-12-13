/* PROTOCOL: I2C CONTROL FUNCTIONS: open, read, write, exchange, spy, close */

/* include the header */
#include "i2c.h"

/* ----------------------------------------------------- */

string I2C::open(HDWF device_handle, int sda, int scl, double clk_rate, bool stretching) {
    /*
        initializes I2C communication

        parameters: - device handle
                    - sda (DIO line used for data)
                    - scl (DIO line used for clock)
                    - rate (clock frequency in Hz, default is 100KHz)
                    - stretching (enables/disables clock stretching)

        returns:    - error message or empty string
    */
    // reset the interface
    FDwfDigitalI2cReset(device_handle);

    // clock stretching
    if (stretching == true) {
        FDwfDigitalI2cStretchSet(device_handle, 1);
    }
    else {
        FDwfDigitalI2cStretchSet(device_handle, 0);
    }

    // set clock frequency
    FDwfDigitalI2cRateSet(device_handle, clk_rate);

    //  set communication lines
    FDwfDigitalI2cSclSet(device_handle, scl);
    FDwfDigitalI2cSdaSet(device_handle, sda);

    // check bus
    int nak = 0;
    FDwfDigitalI2cClear(device_handle, &nak);
    if (nak == 0) {
        string error = "Error: I2C bus lockup";
        return error;
    }


    // write 0 bytes
    FDwfDigitalI2cWrite(device_handle, 0, 0, 0, &nak);
    if (nak != 0) {
        string error = "NAK: index " + to_string(nak);
        return error;
    }
    
    string error = "";
    return error;
}

/* ----------------------------------------------------- */

i2c_data I2C::read(HDWF device_handle, int count, int address) {
    /*
        receives data from I2C
        
        parameters: - device handle
                    - count (number of bytes to receive)
                    - address (8-bit address of the slave device)
        
        return:     - integer list containing the received bytes
                    - error message or empty string
    */
    // create buffer to store data
    vector<unsigned char> buffer(count);

    // receive
    int nak = 0;
    FDwfDigitalI2cRead(device_handle, address << 1, buffer.data(), count, &nak);

    i2c_data data;
    data.data = buffer;
    data.error = "";

    // check for not acknowledged
    if (nak != 0) {
        data.error = "NAK: index " + to_string(nak);
    }
    
    return data;
}

/* ----------------------------------------------------- */

string I2C::write(HDWF device_handle, string data, int address) {
    /*
        send data through I2C
        
        parameters: - device handle
                    - data of type string, int, or list of characters/integers
                    - address (8-bit address of the slave device)
                    
        returns:    - error message or empty string
    */
    // encode the string into a string buffer
    vector<unsigned char> buffer(data.size() + 1);
    vector<char> temporal(data.size() + 1);
    strcpy(temporal.data(), data.c_str());
    for (int index = 0; index < buffer.size(); index++) {
        buffer[index] = (unsigned char)(temporal[index]);
    }

    // send
    int nak = 0;
    FDwfDigitalI2cWrite(device_handle, address << 1, buffer.data(), buffer.size(), &nak);

    // check for not acknowledged
    if (nak != 0) {
        string error = "NAK: index " + to_string(nak);
        return error;
    }
    
    string error = "";
    return error;
}

/* ----------------------------------------------------- */

string I2C::write(HDWF device_handle, vector<unsigned char> data, int address) {
    /*
        send data through I2C
        
        parameters: - device handle
                    - data of type string, int, or list of characters/integers
                    - address (8-bit address of the slave device)
                    
        returns:    - error message or empty string
    */
    // send
    int nak = 0;
    FDwfDigitalI2cWrite(device_handle, address << 1, data.data(), data.size(), &nak);

    // check for not acknowledged
    if (nak != 0) {
        string error = "NAK: index " + to_string(nak);
        return error;
    }
    
    string error = "";
    return error;
}

/* ----------------------------------------------------- */

i2c_data I2C::exchange(HDWF device_handle, string data, int count, int address) {
    /*
        sends and receives data using the I2C interface
        
        parameters: - device handle
                    - data of type string, int, or list of characters/integers
                    - count (number of bytes to receive)
                    - address (8-bit address of the slave device)
        
        return:     - integer list containing the received bytes
                    - error message or empty string
    */
    // create buffer to store data
    vector<unsigned char> rx_buffer(count);

    // encode the string into a string buffer
    vector<unsigned char> tx_buffer(data.size() + 1);
    vector<char> temporal(data.size() + 1);
    strcpy(temporal.data(), data.c_str());
    for (int index = 0; index < tx_buffer.size(); index++) {
        tx_buffer[index] = (unsigned char)(temporal[index]);
    }

    // send and receive
    int nak = 0;
    FDwfDigitalI2cWriteRead(device_handle, address << 1, tx_buffer.data(), tx_buffer.size(), rx_buffer.data(), count, &nak);

    i2c_data out_data;
    out_data.data = rx_buffer;
    out_data.error = "";

    // check for not acknowledged
    if (nak != 0) {
        out_data.error = "NAK: index " + to_string(nak);
    }
    
    return out_data;
}

/* ----------------------------------------------------- */

i2c_data I2C::exchange(HDWF device_handle, vector<unsigned char> data, int count, int address) {
    /*
        sends and receives data using the I2C interface
        
        parameters: - device handle
                    - data of type string, int, or list of characters/integers
                    - count (number of bytes to receive)
                    - address (8-bit address of the slave device)
        
        return:     - integer list containing the received bytes
                    - error message or empty string
    */
    // create buffer to store data
    vector<unsigned char> rx_buffer(count);

    // send and receive
    int nak = 0;
    FDwfDigitalI2cWriteRead(device_handle, address << 1, data.data(), data.size(), rx_buffer.data(), count, &nak);

    i2c_data out_data;
    out_data.data = rx_buffer;
    out_data.error = "";

    // check for not acknowledged
    if (nak != 0) {
        out_data.error = "NAK: index " + to_string(nak);
    }
    
    return out_data;
}

/* ----------------------------------------------------- */

i2c_data I2C::spy(HDWF device_handle, int count) {
    /*
        receives data from I2C
        
        parameters: - device handle
                    - count (number of bytes to receive), default is 16
        
        return:     - class containing the received data: start, address, direction, message, stop
                    - error message or empty string
    */
    // variable to store the results
    i2c_data data;

    // start the interfcae
    FDwfDigitalI2cSpyStart(device_handle);

    // read data
    int start = 0;
    int stop = 0;
    vector<unsigned char> rx_data(count);
    int nak = 0;
    if (FDwfDigitalI2cSpyStatus(device_handle, &start, &stop, rx_data.data(), &count, &nak) == 0) {
        string error = "Communication with the device failed.";
        data.error = error;
    }
    
    // decode data
    if (start != 0) {

        // start condition
        if (start == 1) {
            data.start = "Start";
        }
        else if (start == 2) {
            data.start = "Restart";
        }

        // get address
        data.address = int(rx_data[0] >> 1);

        // decide message direction
        if (rx_data[0] & 1 == 0) {
            data.direction = "Write";
        }
        else {
            data.direction = "Read";
        }
        
        // get message
        data.data = rx_data;

        if (stop != 0) {
            data.stop = "Stop";
        }
    }

    // check for not acknowledged
    if (nak != 0) {
        string error = "NAK: index " + to_string(nak);
        data.error = error;
    }
    
    return data;
}

/* ----------------------------------------------------- */

void I2C::close(HDWF device_handle) {
    /*
        reset the i2c interface
    */
    FDwfDigitalI2cReset(device_handle);
    return;
}
