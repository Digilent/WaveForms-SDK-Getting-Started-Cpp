/* PROTOCOL: SPI CONTROL FUNCTIONS: open, read, write, exchange, close */

/* include the header */
#include "spi.h"

/* ----------------------------------------------------- */

void wf::SPI::open(Device::Data device_data, int cs, int sck, int miso, int mosi, double clk_frequency, int mode, bool order) {
    /*
        initializes SPI communication

        parameters: - device data
                    - cs (DIO line used for chip select)
                    - sck (DIO line used for serial clock)
                    - miso (DIO line used for master in - slave out, optional)
                    - mosi (DIO line used for master out - slave in, optional)
                    - frequency (communication frequency in Hz, default is 1MHz)
                    - mode (SPI mode: 0: CPOL=0, CPHA=0; 1: CPOL-0, CPHA=1; 2: CPOL=1, CPHA=0; 3: CPOL=1, CPHA=1)
                    - order (endianness, True means MSB first - default, False means LSB first)
    */
    // set the clock frequency
    FDwfDigitalSpiFrequencySet(device_data.handle, clk_frequency);

    // set the clock pin
    FDwfDigitalSpiClockSet(device_data.handle, sck);

    if (mosi != -1) {
        // set the mosi pin
        FDwfDigitalSpiDataSet(device_data.handle, 0, mosi);

        // set the initial state
        FDwfDigitalSpiIdleSet(device_data.handle, 0, DwfDigitalOutIdleZet);
    }

    if (miso != -1) {
        // set the miso pin
        FDwfDigitalSpiDataSet(device_data.handle, 1, miso);

        // set the initial state
        FDwfDigitalSpiIdleSet(device_data.handle, 1, DwfDigitalOutIdleZet);
    }

    // set the SPI mode
    FDwfDigitalSpiModeSet(device_data.handle, mode);

    // set endianness
    FDwfDigitalSpiOrderSet(device_data.handle, int(order));

    // set the cs pin HIGH
    FDwfDigitalSpiSelect(device_data.handle, cs, 1);

    // dummy write
    FDwfDigitalSpiWriteOne(device_data.handle, 1, 0, 0);
    state.on = true;
    state.off = false;
    state.frequency = clk_frequency;
    state.order = order;
    state.mode = mode;
    return;
}

/* ----------------------------------------------------- */

std::vector<unsigned char> wf::SPI::read(Device::Data device_data, int count, int cs) {
    /*
        receives data from SPI

        parameters: - device handle
                    - count (number of bytes to receive)
                    - chip select line number

        return:     - integer list containing the received bytes
    */
    // enable the chip select line
    FDwfDigitalSpiSelect(device_data.handle, cs, 0);

    // create buffer to store data
    std::vector<unsigned char> buffer(count);

    // read array of 8 bit elements
    FDwfDigitalSpiRead(device_data.handle, 1, 8, buffer.data(), buffer.size());

    // disable the chip select line
    FDwfDigitalSpiSelect(device_data.handle, cs, 1);

    return buffer;
}

/* ----------------------------------------------------- */

void wf::SPI::write(Device::Data device_data, std::string data, int cs) {
    /*
        send data through SPI

        parameters: - device handle
                    - data of type string
                    - chip select line number
    */
    // create buffer to write
    std::vector<unsigned char> buffer(data.size() + 1);
    for (int index = 0; index < buffer.size(); index++) {
        buffer[index] = (unsigned char)(data[index]);
    }
    write(device_data, buffer, cs);
    return;
}

/* ----------------------------------------------------- */

void wf::SPI::write(Device::Data device_data, std::vector<unsigned char> data, int cs) {
    /*
        send data through SPI

        parameters: - device handle
                    - data of type list of characters/integers
                    - chip select line number
    */
    // enable the chip select line
    FDwfDigitalSpiSelect(device_data.handle, cs, 0);

    // write array of 8 bit elements
    FDwfDigitalSpiWrite(device_data.handle, 1, 8, data.data(), data.size());

    // disable the chip select line
    FDwfDigitalSpiSelect(device_data.handle, cs, 1);
    return;
}

/* ----------------------------------------------------- */

std::vector<unsigned char> wf::SPI::exchange(Device::Data device_data, std::string tx_data, int count, int cs) {
    /*
        sends and receives data using the SPI interface
        
        parameters: - device handle
                    - data of type string
                    - count (number of bytes to receive)
                    - chip select line number
        
        return:     - integer list containing the received bytes
    */
    // create buffer to write
    std::vector<unsigned char> buffer(tx_data.size() + 1);
    for (int index = 0; index < buffer.size(); index++) {
        buffer[index] = (unsigned char)(tx_data[index]);
    }
    return exchange(device_data, buffer, count, cs);
}

/* ----------------------------------------------------- */

std::vector<unsigned char> wf::SPI::exchange(Device::Data device_data, std::vector<unsigned char> tx_data, int count, int cs) {
    /*
        sends and receives data using the SPI interface
        
        parameters: - device handle
                    - data of type list of characters/integers
                    - count (number of bytes to receive)
                    - chip select line number
        
        return:     - integer list containing the received bytes
    */
    // enable the chip select line
    FDwfDigitalSpiSelect(device_data.handle, cs, 0);

    // create buffer to store data
    std::vector<unsigned char> rx_data(count);

    // write to MOSI and read from MISO
    FDwfDigitalSpiWriteRead(device_data.handle, 1, 8, tx_data.data(), tx_data.size(), rx_data.data(), rx_data.size());

    // disable the chip select line
    FDwfDigitalSpiSelect(device_data.handle, cs, 1);

    // decode data
    return rx_data;
}

/* ----------------------------------------------------- */

//spi_data wf::SPI::spy(Device::Data device_data, int count, int cs, int sck, int mosi, int miso, int word_size) {
    /*
        receives data from SPI

        parameters: - device handle
                    - count (number of bytes to receive)
                    - chip select line number
                    - serial clock line number
                    - master out - slave in - optional
                    - master in - slave out - optional
                    - word size in bits (default is 8)

        returns:    - class containing the received data: mosi, miso, errors
    */
    // variable to store results
/*    spi_data data;
    unsigned long long temp_mosi = 0;
    unsigned long long temp_miso = 0;

    // record mode
    FDwfDigitalInAcquisitionModeSet(device_data.handle, acqmodeRecord);

    // for sync mode set divider to -1 
    FDwfDigitalInDividerSet(device_data.handle, -1);

    // 8 bit per sample format, DIO 0-7
    FDwfDigitalInSampleFormatSet(device_data.handle, 8);

    // continuous sampling 
    FDwfDigitalInTriggerPositionSet(device_data.handle, -1);

    // in sync mode the trigger is used for sampling condition
    // trigger detector mask: low & high & (rising | falling)
    FDwfDigitalInTriggerSet(device_data.handle, 0, 0, (1 << sck) | (1 << cs), 0);
    // sample on clock rising edge for sampling bits, or CS rising edge to detect frames

    // start detection
    FDwfDigitalInConfigure(device_data.handle, 0, 1);

    // fill buffer
    unsigned char status = 0;
    int available = 0;
    int lost = 0;
    int corrupted = 0;
    FDwfDigitalInStatus(device_data.handle, 1, &status);
    FDwfDigitalInStatusRecord(device_data.handle, &available, &lost, &corrupted);

    // check data integrity
    if (lost != 0) {
        data.error = "Samples were lost";
    }
    if (corrupted != 0) {
        data.error = "Samples could be corrupted";
    }

    // limit data size
    if (available > count) {
        available = count;
    }
    
    // load data from internal buffer
    vector<unsigned char> rx_data(available);
    FDwfDigitalInStatusData(device_data.handle, rx_data.data(), available);

    // get message
    int bit_count = 0;
    for (int index = 0; index < available; index++) {
        
        // CS low, active
        if ((rx_data[index] >> cs) & 1) {
            // log leftover bits, frame not multiple of bit count
            if (bit_count != 0) {
                data.mosi = convert(temp_mosi);
                data.miso = convert(temp_miso);
                return data;
            }
        }

        // CS low, active
        else {
            bit_count += 1;  // increment bit count

            temp_mosi <<= 1; // shift existing bits
            temp_miso <<= 1;

            // check outgoing data
            if ((rx_data[index] >> mosi) & 1) {
                temp_mosi |= 1;
            }
            
            // check incoming data
            if ((rx_data[index] >> miso) & 1) {
                temp_miso |= 1;
            }

            // got nBits of bits
            if (bit_count >= word_size) {
                data.mosi = convert(temp_mosi);
                data.miso = convert(temp_miso);
                return data;
            }
        }
    }

    data.mosi = convert(temp_mosi);
    data.miso = convert(temp_miso);
    return data;
}*/

/* ----------------------------------------------------- */

void wf::SPI::close(Device::Data device_data) {
    /*
        reset the spi interface
    */
    FDwfDigitalSpiReset(device_data.handle);
    state.on = false;
    state.off = true;
    return;
}

/* ----------------------------------------------------- */

std::vector<unsigned char> wf::SPI::convert(unsigned long long number) {
    /* convert a number to a byte array */
    int index = 0;
    std::vector<unsigned char> array;
    if (number == 0) {
        array.insert(array.end(), 0);
    }
    else {
        while (number > 0) {
            array.insert(array.end(), number & 0xFF);
            number >>= 8;
        }
    }
    return array;
}
