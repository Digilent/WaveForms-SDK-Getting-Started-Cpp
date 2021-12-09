/* PROTOCOL: I2C CONTROL FUNCTIONS: open, read, write, exchange, spy, close */

/* include the header */
#include "i2c.h"

/* ----------------------------------------------------- */

string I2C::open(HDWF device_handle, int sda, int scl, double clk_rate, bool stretching) {

}

/* ----------------------------------------------------- */

i2c_data I2C::read(HDWF device_handle, int count, int address) {

}

/* ----------------------------------------------------- */

string I2C::write(HDWF device_handle, string data, int address) {

}

/* ----------------------------------------------------- */

string I2C::write(HDWF device_handle, int* data, int address) {

}

/* ----------------------------------------------------- */

i2c_data I2C::exchange(HDWF device_handle, string data, int count, int address) {

}

/* ----------------------------------------------------- */

i2c_data I2C::exchange(HDWF device_handle, int* data, int count, int address) {

}

/* ----------------------------------------------------- */

i2c_data I2C::spy(HDWF device_handle, int count) {

}

/* ----------------------------------------------------- */

void I2C::close(HDWF device_handle) {

}
