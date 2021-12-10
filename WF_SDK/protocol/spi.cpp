/* PROTOCOL: SPI CONTROL FUNCTIONS: open, read, write, exchange, spy, close */

/* include the header */
#include "spi.h"

/* ----------------------------------------------------- */

void SPI::open(HDWF device_handle, int cs, int sck, int miso, int mosi, double clk_frequency, int mode, bool order) {
    /*
        initializes SPI communication

        parameters: - device handle
                    - cs (DIO line used for chip select)
                    - sck (DIO line used for serial clock)
                    - miso (DIO line used for master in - slave out, optional)
                    - mosi (DIO line used for master out - slave in, optional)
                    - frequency (communication frequency in Hz, default is 1MHz)
                    - mode (SPI mode: 0: CPOL=0, CPHA=0; 1: CPOL-0, CPHA=1; 2: CPOL=1, CPHA=0; 3: CPOL=1, CPHA=1)
                    - order (endianness, True means MSB first - default, False means LSB first)
    */
    // set the clock frequency
    FDwfDigitalSpiFrequencySet(device_handle, clk_frequency);

    // set the clock pin
    FDwfDigitalSpiClockSet(device_handle, sck);

    if (mosi != -1) {
        // set the mosi pin
        FDwfDigitalSpiDataSet(device_handle, 0, mosi);

        // set the initial state
        FDwfDigitalSpiIdleSet(device_handle, 0, DwfDigitalOutIdleZet);
    }

    if (miso != -1) {
        // set the miso pin
        FDwfDigitalSpiDataSet(device_handle, 1, miso);

        // set the initial state
        FDwfDigitalSpiIdleSet(device_handle, 1, DwfDigitalOutIdleZet);
    }

    // set the SPI mode
    FDwfDigitalSpiModeSet(device_handle, mode);

    // set endianness
    FDwfDigitalSpiOrderSet(device_handle, int(order));

    // set the cs pin HIGH
    FDwfDigitalSpiSelect(device_handle, cs, 1);

    // dummy write
    FDwfDigitalSpiWriteOne(device_handle, 1, 0, 0);
    return;
}

/* ----------------------------------------------------- */

unsigned char* SPI::read(HDWF device_handle, int count, int cs) {
    /*
        receives data from SPI

        parameters: - device handle
                    - count (number of bytes to receive)
                    - chip select line number

        return:     - integer list containing the received bytes
    */
    // enable the chip select line
    FDwfDigitalSpiSelect(device_handle, cs, 0);

    // create buffer to store data
    unsigned char buffer[count];

    // read array of 8 bit elements
    FDwfDigitalSpiRead(device_handle, 1, 8, buffer, length(buffer));

    // disable the chip select line
    FDwfDigitalSpiSelect(device_handle, cs, 1);

    return buffer;
}

/* ----------------------------------------------------- */

void SPI::write(HDWF device_handle, string data, int cs) {
    /*
        send data through SPI

        parameters: - device handle
                    - data of type string
                    - chip select line number
    */
    // enable the chip select line
    FDwfDigitalSpiSelect(device_handle, cs, 0);

    // create buffer to write
    unsigned char buffer[data.size() + 1];
    char temporal[data.size() + 1];
    strcpy(temporal, data.c_str());
    for (int index = 0; index < length(buffer); index++) {
        buffer[index] = (unsigned char)(temporal[index]);
    }

    // write array of 8 bit elements
    FDwfDigitalSpiWrite(device_handle, 1, 8, buffer, length(buffer));

    // disable the chip select line
    FDwfDigitalSpiSelect(device_handle, cs, 1);
    return;
}

/* ----------------------------------------------------- */

void SPI::write(HDWF device_handle, unsigned char* data, int cs) {
    /*
        send data through SPI

        parameters: - device handle
                    - data of type list of characters/integers
                    - chip select line number
    */
    // enable the chip select line
    FDwfDigitalSpiSelect(device_handle, cs, 0);

    // create buffer to write
    unsigned char buffer[length(data)];
    for (int index = 0; index < length(buffer); index++) {
        buffer[index] = (unsigned char)(data[index]);
    }

    // write array of 8 bit elements
    FDwfDigitalSpiWrite(device_handle, 1, 8, buffer, length(buffer));

    // disable the chip select line
    FDwfDigitalSpiSelect(device_handle, cs, 1);
    return;
}

/* ----------------------------------------------------- */

unsigned char* SPI::exchange(HDWF device_handle, string data, int count, int cs) {
    /*
        sends and receives data using the SPI interface
        
        parameters: - device handle
                    - data of type string
                    - count (number of bytes to receive)
                    - chip select line number
        
        return:     - integer list containing the received bytes
    */
    // enable the chip select line
    FDwfDigitalSpiSelect(device_handle, cs, 0);

    // create buffer to write
    unsigned char tx_buffer[data.size() + 1];
    char temporal[data.size() + 1];
    strcpy(temporal, data.c_str());
    for (int index = 0; index < length(tx_buffer); index++) {
        tx_buffer[index] = (unsigned char)(temporal[index]);
    }

    // create buffer to store data
    unsigned char rx_buffer[count];

    // write to MOSI and read from MISO
    FDwfDigitalSpiWriteRead(device_handle, 1, 8, tx_buffer, length(tx_buffer), rx_buffer, length(rx_buffer));

    // disable the chip select line
    FDwfDigitalSpiSelect(device_handle, cs, 1);

    // decode data
    return rx_buffer;
}

/* ----------------------------------------------------- */

unsigned char* SPI::exchange(HDWF device_handle, unsigned char* data, int count, int cs) {
    /*
        sends and receives data using the SPI interface
        
        parameters: - device handle
                    - data of type list of characters/integers
                    - count (number of bytes to receive)
                    - chip select line number
        
        return:     - integer list containing the received bytes
    */
    // enable the chip select line
    FDwfDigitalSpiSelect(device_handle, cs, 0);

    // create buffer to write
    unsigned char tx_buffer[length(data)];
    for (int index = 0; index < length(tx_buffer); index++) {
        tx_buffer[index] = (unsigned char)(data[index]);
    }

    // create buffer to store data
    unsigned char rx_buffer[count];

    // write to MOSI and read from MISO
    FDwfDigitalSpiWriteRead(device_handle, 1, 8, tx_buffer, length(tx_buffer), rx_buffer, length(rx_buffer));

    // disable the chip select line
    FDwfDigitalSpiSelect(device_handle, cs, 1);

    // decode data
    return rx_buffer;
}

/* ----------------------------------------------------- */

spi_data SPI::spy(HDWF device_handle, int count, int cs, int sck, int mosi, int miso, int word_size) {
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
    spi_data data;
    unsigned long long temp_mosi = 0;
    unsigned long long temp_miso = 0;

    // record mode
    FDwfDigitalInAcquisitionModeSet(device_handle, acqmodeRecord);

    // for sync mode set divider to -1 
    FDwfDigitalInDividerSet(device_handle, -1);

    // 8 bit per sample format, DIO 0-7
    FDwfDigitalInSampleFormatSet(device_handle, 8);

    // continuous sampling 
    FDwfDigitalInTriggerPositionSet(device_handle, -1);

    // in sync mode the trigger is used for sampling condition
    // trigger detector mask: low & high & (rising | falling)
    FDwfDigitalInTriggerSet(device_handle, 0, 0, (1 << sck) | (1 << cs), 0);
    // sample on clock rising edge for sampling bits, or CS rising edge to detect frames

    // start detection
    FDwfDigitalInConfigure(device_handle, 0, 1);

    // fill buffer
    unsigned char status = 0;
    int available = 0;
    int lost = 0;
    int corrupted = 0;
    FDwfDigitalInStatus(device_handle, 1, &status);
    FDwfDigitalInStatusRecord(device_handle, &available, &lost, &corrupted);

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
    unsigned char rx_data[available];
    FDwfDigitalInStatusData(device_handle, rx_data, available);

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
}

/* ----------------------------------------------------- */

void SPI::close(HDWF device_handle) {
    /*
        reset the spi interface
    */
    FDwfDigitalSpiReset(device_handle);
    return;
}

/* ----------------------------------------------------- */

int SPI::length(unsigned char* array) {
    /* get the length of a byte array */
    return (sizeof(array) / sizeof(array[0]));
}

/* ----------------------------------------------------- */

unsigned char* SPI::convert(unsigned long long number) {
    /* convert a number to a byte array */
    int index = 0;
    unsigned char array[] = {};
    if (number == 0) {
        array[0] = 0;
    }
    else {
        while (number > 0) {
            array[index] = number & 0xFF;
            number >>= 8;
        }
    }
    return array;
}
