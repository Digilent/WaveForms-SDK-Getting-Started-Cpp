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

template <typename T>
inline T const& Tools::min(T const& a, T const& b) {
    if (a < b) {
        return a;
    }
    return b;
}

/* ----------------------------------------------------- */

template <typename T>
inline T const& Tools::max(T const& a, T const& b) {
    if (a > b) {
        return a;
    }
    return b;
}

/* ----------------------------------------------------- */

std::vector<double> Tools::spectrum(std::vector<double> buffer, DwfWindow window, double sample_rate, double frequency_start, double frequency_stop) {
    // get and apply window
    int buffer_length = buffer.size();
    std::vector<double> window_buffer(buffer_length);
    FDwfSpectrumWindow(window_buffer.data(), buffer_length, window, 1.0, NULL);
    for (int index = 0; index < buffer_length; index++) {
        buffer[index] *= window_buffer[index];
    }

    // get the spectrum
    int spectrum_length = buffer_length / 2 + 1;
    std::vector<double> spectrum(spectrum_length);
    frequency_start = max(frequency_start * 2.0 / sample_rate, 0.0);
    frequency_stop = min(frequency_stop * 2.0 / sample_rate, 1.0);
    FDwfSpectrumTransform(buffer.data(), buffer_length, spectrum.data(), NULL, spectrum_length, frequency_start, frequency_stop);
    for (int index = 0; index < spectrum_length; index++) {
        spectrum[index] = 20.0 * log10(spectrum[index] / sqrt(2));
    }
    return spectrum;
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
