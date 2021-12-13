/* PROTOCOL: SPI CONTROL FUNCTIONS: open, read, write, exchange, spy, close */

/* include the necessary libraries */
#include <string>
#include <string.h>
#include <vector>

using namespace std;

/* include the constants and the WaveForms function library */
#ifdef _WIN32
#include "C:/Program Files (x86)/Digilent/WaveFormsSDK/inc/dwf.h"
#elif __APPLE__
#include "/Library/Frameworks/dwf.framework/Headers/dwf.h"
#else
#include <digilent/waveforms/dwf.h>
#endif

/* ----------------------------------------------------- */

class spi_data {
    public:
        vector<unsigned char> mosi;
        vector<unsigned char> miso;
        string error = "";
        spi_data& operator=(const spi_data&);
};

spi_data& spi_data::operator=(const spi_data &data) {
    if (this != &data) {
        mosi = data.mosi;
        miso = data.miso;
        error = data.error;
    }
    return *this;
}

/* ----------------------------------------------------- */

class SPI {
    private:
        vector<unsigned char> convert(unsigned long long number);
    public:
        void open(HDWF device_handle, int cs, int sck, int miso = -1, int mosi = -1, double clk_frequency = 1e06, int mode = 0, bool order = true);
        vector<unsigned char> read(HDWF device_handle, int count, int cs);
        void write(HDWF device_handle, string data, int cs);
        void write(HDWF device_handle, vector<unsigned char> data, int cs);
        vector<unsigned char> exchange(HDWF device_handle, string data, int count, int cs);
        vector<unsigned char> exchange(HDWF device_handle, vector<unsigned char> data, int count, int cs);
        spi_data spy(HDWF device_handle, int count, int cs, int sck, int mosi = -1, int miso = -1, int word_size = 8);
        void close(HDWF device_handle);
} spi;
