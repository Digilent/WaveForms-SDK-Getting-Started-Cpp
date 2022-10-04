/* PROTOCOL: I2C CONTROL FUNCTIONS: open, read, write, exchange, close */

/* include the header */
#include "i2c.h"

/* ----------------------------------------------------- */

void wf::I2C::open(Device::Data *device_data, int sda, int scl, double clk_rate, bool stretching) {
    /*
        initializes I2C communication

        parameters: - device data
                    - sda (DIO line used for data)
                    - scl (DIO line used for clock)
                    - rate (clock frequency in Hz, default is 100KHz)
                    - stretching (enables/disables clock stretching)
    */
    // reset the interface
    if (FDwfDigitalI2cReset(device_data->handle) == 0) {
        device.check_error(device_data);
    }

    // clock stretching
    if (stretching == true) {
        if (FDwfDigitalI2cStretchSet(device_data->handle, 1) == 0) {
            device.check_error(device_data);
        }
    }
    else {
        if (FDwfDigitalI2cStretchSet(device_data->handle, 0) == 0) {
            device.check_error(device_data);
        }
    }

    // set clock frequency
    if (FDwfDigitalI2cRateSet(device_data->handle, clk_rate) == 0) {
        device.check_error(device_data);
    }

    //  set communication lines
    if (FDwfDigitalI2cSclSet(device_data->handle, scl) == 0) {
        device.check_error(device_data);
    }
    if (FDwfDigitalI2cSdaSet(device_data->handle, sda) == 0) {
        device.check_error(device_data);
    }

    // check bus
    int nak = 0;
    if (FDwfDigitalI2cClear(device_data->handle, &nak) == 0) {
        device.check_error(device_data);
    }
    if (nak == 0) {
        device_data->warning.instrument = "protocol/i2c";
        device_data->warning.function = "open";
        device_data->warning.message = "I2C bus lockup";
        throw device_data->warning;
    }


    // write 0 bytes
    if (FDwfDigitalI2cWrite(device_data->handle, 0, 0, 0, &nak) == 0) {
        device.check_error(device_data);
    }
    check_warning(device_data, nak);
    return;
}

/* ----------------------------------------------------- */

std::vector<unsigned char> wf::I2C::read(Device::Data *device_data, int count, int address) {
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
    if (FDwfDigitalI2cRead(device_data->handle, address << 1, data.data(), data.size(), &nak) == 0) {
        device.check_error(device_data);
    }

    // check for not acknowledged
    check_warning(device_data, nak);
    return data;
}

/* ----------------------------------------------------- */

void wf::I2C::write(Device::Data *device_data, std::string data, int address) {
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
    write(device_data, buffer, address);
    return;
}

/* ----------------------------------------------------- */

void wf::I2C::write(Device::Data *device_data, std::vector<unsigned char> data, int address) {
    /*
        send data through I2C
        
        parameters: - device handle
                    - data of type string, int, or list of characters/integers
                    - address (8-bit address of the slave device)
                    - pointer to error string
    */
    // send
    int nak = 0;
    if (FDwfDigitalI2cWrite(device_data->handle, address << 1, data.data(), data.size(), &nak) == 0) {
        device.check_error(device_data);
    }

    // check for not acknowledged
    check_warning(device_data, nak);
    return;
}

/* ----------------------------------------------------- */

std::vector<unsigned char> wf::I2C::exchange(Device::Data *device_data, std::string tx_data, int count, int address) {
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

    return exchange(device_data, buffer, count, address);
}

/* ----------------------------------------------------- */

std::vector<unsigned char> wf::I2C::exchange(Device::Data *device_data, std::vector<unsigned char> tx_data, int count, int address) {
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
    if (FDwfDigitalI2cWriteRead(device_data->handle, address << 1, tx_data.data(), tx_data.size(), rx_data.data(), rx_data.size(), &nak) == 0) {
        device.check_error(device_data);
    }

    // check for not acknowledged
    check_warning(device_data, nak);
    return rx_data;
}

/* ----------------------------------------------------- */

//std::string wf::I2C::spy(Device::Data device_data, int count, std::string *error) {
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
    if (FDwfDigitalI2cSpyStart(device_data->handle);

    // read data
    int start = 0;
    int stop = 0;
    vector<unsigned char> rx_data(count);
    int nak = 0;
    if (if (FDwfDigitalI2cSpyStatus(device_data->handle, &start, &stop, rx_data.data(), &count, &nak) == 0) {
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

void wf::I2C::close(Device::Data *device_data) {
    /*
        reset the i2c interface
    */
    if (FDwfDigitalI2cReset(device_data->handle) == 0) {
        device.check_error(device_data);
    }
    return;
}

/* ----------------------------------------------------- */

void wf::I2C::check_warning(Device::Data *device_data, int nak, const char *caller) {
    /*
        check for I2C errors
    */
    if (nak != 0) {
        device_data->warning.instrument = "protocol/i2c";
        device_data->warning.function = caller;
        device_data->warning.message = " -> NAK: index " + std::to_string(nak);
        throw device_data->warning;
    }
    return;
}
