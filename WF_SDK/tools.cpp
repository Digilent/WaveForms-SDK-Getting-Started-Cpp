/* TOOLS: sleep, keyboard_interrupt_reset */

/* include the header */
#include "tools.h"

/* ----------------------------------------------------- */

void Tools::sleep(int millis) {
    int start = get_time();
    int current = get_time();
    while (current - start < millis) {
        current = get_time();
    }
    return;
}

/* ----------------------------------------------------- */

int Tools::get_time(void) {
    auto time = std::chrono::system_clock::now();
    auto duration = time.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    int now = (int)millis.count();
    return now;
}

/* ----------------------------------------------------- */

void Tools::keyboard_interrupt_reset(Device::Data device_data) {
    device_handle = device_data.handle;
    signal(SIGINT, ISR);
    return;
}

/* ----------------------------------------------------- */

void ISR(int signum) {
    // reset the scope
    FDwfAnalogInReset(device_handle);
    // reset the wavegen
    FDwfAnalogOutReset(device_handle, -1);
    // stop and reset the power supplies
    FDwfAnalogIOEnableSet(device_handle, 0);
    FDwfAnalogIOReset(device_handle);
    // reset the DMM
    FDwfAnalogIOReset(device_handle);
    // reset the logic analyzer
    FDwfDigitalInReset(device_handle);
    // reset the pattern generator
    FDwfDigitalOutReset(device_handle);
    // reset the static IO
    FDwfDigitalIOReset(device_handle);
    // reset the UART interface
    FDwfDigitalUartReset(device_handle);
    // reset the SPI interface
    FDwfDigitalSpiReset(device_handle);
    // reset the I2C interface
    FDwfDigitalI2cReset(device_handle);
    // close the connection
    FDwfDeviceClose(device_handle);
    exit(0);
    return;
}
