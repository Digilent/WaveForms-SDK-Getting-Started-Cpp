/* PROTOCOL: UART CONTROL FUNCTIONS: open, read, write, close */

/* include the necessary libraries */
#include <string>
#include <string.h>

using namespace std;

/* include the constants and the WaveForms function library */
#ifdef WIN32
#include "C:/Program Files (x86)/Digilent/WaveFormsSDK/inc/dwf.h"
#elif __APPLE__
#include "/Library/Frameworks/dwf.framework/Headers/dwf.h"
#else
#include <digilent/waveforms/dwf.h>
#endif

/* ----------------------------------------------------- */

struct UART_Data {
    unsigned char* data;
    string error;
};

typedef struct UART_Data uart_data;

/* ----------------------------------------------------- */

class UART {
    public:
        void open(HDWF device_handle, int rx, int tx, int baud_rate = 9600, bool parity = -1, int data_bits = 8, int stop_bits = 1);
        uart_data read(HDWF device_handle);
        void write(HDWF device_handle, string data);
        void write(HDWF device_handle, unsigned char* data);
        void close(HDWF device_handle);
} uart;
