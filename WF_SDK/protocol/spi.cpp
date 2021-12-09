/* PROTOCOL: SPI CONTROL FUNCTIONS: open, read, write, exchange, spy, close */

/* include the header */
#include "spi.h"

/* ----------------------------------------------------- */

void SPI::open(HDWF device_handle, int cs, int sck, int miso, int mosi, double clk_frequency, int mode, bool order) {

}

/* ----------------------------------------------------- */

int* SPI::read(HDWF device_handle, int count, int cs) {

}

/* ----------------------------------------------------- */

void SPI::write(HDWF device_handle, string data, int cs) {

}

/* ----------------------------------------------------- */

void SPI::write(HDWF device_handle, int* data, int cs) {

}

/* ----------------------------------------------------- */

int* SPI::exchange(HDWF device_handle, string data, int count, int cs) {

}

/* ----------------------------------------------------- */

int* SPI::exchange(HDWF device_handle, int* data, int count, int cs) {

}

/* ----------------------------------------------------- */

spi_data SPI::spy(HDWF device_handle, int count, int cs, int sck, int mosi, int miso, int word_size) {

}

/* ----------------------------------------------------- */

void SPI::close(HDWF device_handle) {

}
