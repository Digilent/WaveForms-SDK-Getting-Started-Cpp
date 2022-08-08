/* PROTOCOL: I2C CONTROL FUNCTIONS: open, read, write, exchange, close */

/* include the header */
#include "i2c.h"

/* ----------------------------------------------------- */

void I2C::open(Device::Data device_data, int sda, int scl, double clk_rate, bool stretching, std::string *error) {
    /*
        initializes I2C communication

        parameters: - device data
                    - sda (DIO line used for data)
                    - scl (DIO line used for clock)
                    - rate (clock frequency in Hz, default is 100KHz)
                    - stretching (enables/disables clock stretching)
    */
    // reset the interface
    FDwfDigitalI2cReset(device_data.handle);

    // clock stretching
    if (stretching == true) {
        FDwfDigitalI2cStretchSet(device_data.handle, 1);
    }
    else {
        FDwfDigitalI2cStretchSet(device_data.handle, 0);
    }

    // set clock frequency
    FDwfDigitalI2cRateSet(device_data.handle, clk_rate);
    state.frequency = clk_rate;

    //  set communication lines
    FDwfDigitalI2cSclSet(device_data.handle, scl);
    FDwfDigitalI2cSdaSet(device_data.handle, sda);

    // check bus
    int nak = 0;
    FDwfDigitalI2cClear(device_data.handle, &nak);
    if (nak == 0) {
        *error = "Error: I2C bus lockup";
        return;
    }


    // write 0 bytes
    FDwfDigitalI2cWrite(device_data.handle, 0, 0, 0, &nak);
    if (nak != 0) {
        *error = "NAK: index " + std::to_string(nak);
        return;
    }
    
    state.on = true;
    state.off = false;
    *error = "";
    return;
}

/* ----------------------------------------------------- */

std::vector<unsigned char> I2C::read(Device::Data device_data, int count, int address, std::string *error) {
    /*
        receives data from I2C
        
        parameters: - device data
                    - number of bytes to receive
                    - address (8-bit address of the slave device)
                    - pointer to error string
        
        return:     - integer list of received data bytes
    */
    // receive
    int nak = 0;
    std::vector<unsigned char> data(count);
    FDwfDigitalI2cRead(device_data.handle, address << 1, data.data(), data.size(), &nak);

    // check for not acknowledged
    *error = "";
    if (nak != 0) {
        *error = "NAK: index " + std::to_string(nak);
    }
    
    return data;
}

/* ----------------------------------------------------- */

void I2C::write(Device::Data device_data, std::string data, int address, std::string *error) {
    /*
        send data through I2C
        
        parameters: - device handle
                    - data of type string, int, or list of characters/integers
                    - address (8-bit address of the slave device)
                    - pointer to error string
    */
    // encode the string into a string buffer
    std::vector<unsigned char> buffer(data.size() + 1);
    for (int index = 0; index < buffer.size(); index++) {
        buffer[index] = (unsigned char)(data[index]);
    }
    write(device_data, buffer, address, error);
    return;
}

/* ----------------------------------------------------- */

void I2C::write(Device::Data device_data, std::vector<unsigned char> data, int address, std::string *error) {
    /*
        send data through I2C
        
        parameters: - device handle
                    - data of type string, int, or list of characters/integers
                    - address (8-bit address of the slave device)
                    - pointer to error string
    */
    // send
    int nak = 0;
    FDwfDigitalI2cWrite(device_data.handle, address << 1, data.data(), data.size(), &nak);

    // check for not acknowledged
    *error = "";
    if (nak != 0) {
        *error = "NAK: index " + std::to_string(nak);
    }
    
    return;
}

/* ----------------------------------------------------- */

std::vector<unsigned char> I2C::exchange(Device::Data device_data, std::string tx_data, int count, int address, std::string *error) {
    /*
        sends and receives data using the I2C interface
        
        parameters: - device handle
                    - data of type string, int, or list of characters/integers
                    - number of bytes to receive
                    - address (8-bit address of the slave device)
                    - pointer to error string
        
        return:     - integer list of received bytes
    */
    // encode the string into a string buffer
    std::vector<unsigned char> buffer(tx_data.size() + 1);
    for (int index = 0; index < buffer.size(); index++) {
        buffer[index] = (unsigned char)(tx_data[index]);
    }

    return exchange(device_data, buffer, count, address, error);
}

/* ----------------------------------------------------- */

std::vector<unsigned char> I2C::exchange(Device::Data device_data, std::vector<unsigned char> tx_data, int count, int address, std::string *error) {
    /*
        sends and receives data using the I2C interface
        
        parameters: - device handle
                    - data of type string, int, or list of characters/integers
                    - number of bytes to receive
                    - address (8-bit address of the slave device)
        
        return:     - integer list of received bytes
    */
    // send and receive
    int nak = 0;
    std::vector<unsigned char> rx_data(count);
    FDwfDigitalI2cWriteRead(device_data.handle, address << 1, tx_data.data(), tx_data.size(), rx_data.data(), rx_data.size(), &nak);

    // check for not acknowledged
    *error = "";
    if (nak != 0) {
        *error = "NAK: index " + std::to_string(nak);
    }
    return rx_data;
}

/* ----------------------------------------------------- */

//std::string I2C::spy(Device::Data device_data, int count, std::string *error) {
    /*
        receives data from I2C
        
        parameters: - device handle
                    - count (number of bytes to receive), default is 16
        
        return:     - class containing the received data: start, address, direction, message, stop
                    - error message or empty string
    */
    // variable to store the results
/*    i2c_data data;

    // start the interfcae
    FDwfDigitalI2cSpyStart(device_data.handle);

    // read data
    int start = 0;
    int stop = 0;
    vector<unsigned char> rx_data(count);
    int nak = 0;
    if (FDwfDigitalI2cSpyStatus(device_data.handle, &start, &stop, rx_data.data(), &count, &nak) == 0) {
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
}*/

/* ----------------------------------------------------- */

void I2C::close(Device::Data device_data) {
    /*
        reset the i2c interface
    */
    FDwfDigitalI2cReset(device_data.handle);
    state.on = false;
    state.off = true;
    return;
}
