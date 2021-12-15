#include "WF_SDK/WF_SDK.h"  // include all classes and functions

#include <iostream>         // needed for input/output
#include <signal.h>         // needed for keyboard interrupt handling
#include <vector>           // needed for vector handling
#include <sstream>          // needed for string manipulation

using namespace std;

// macros
#ifdef _WIN32
#   include <windows.h>
#   define sleep(time) Sleep((int)(time))
#else
#   include <unistd.h>
#   include <sys/time.h>
#   define sleep(time) usleep((int)(1000 * time))
#endif

// function prototypes
void signal_handler(int sig_num);

// global variables
device_data this_device;
supplies_data supplies_state;

// define DIO lines
int sda = 0;
int scl = 1;

// define addresses
int CLS_address = 0x48;
int TMP2_address = 0x4B;

/* ----------------------------------------------------- */

void main(void) {
    // connect to the device
    device_data this_device;
    this_device = device.open();

    // check for connection errors
    device.check_error(this_device.handle);

    /* ----------------------------------------------------- */

    // start the power supplies
    supplies_state.name = this_device.name;
    supplies_state.master_state = true;
    supplies_state.state = true;
    supplies_state.voltage = 3.3;
    supplies.switch_(this_device.handle, supplies_state);
    sleep(100);    // delay

    // initialize the i2c interface on DIO0 and DIO1
    i2c.open(this_device.handle, sda, scl);

    // initialize the PMOD TMP2 (set output size to 16-bit)
    vector<unsigned char> input_data {0x03, 0x80};
    i2c.write(this_device.handle, input_data, TMP2_address);

    // save custom character
    i2c.write(this_device.handle, "\x1b[7;5;7;0;0;0;0;0;0d", CLS_address);   // define character
    i2c.write(this_device.handle, "\x1b[3p", CLS_address); // load character table

    signal(SIGINT, signal_handler);

    cout << "Press Ctrl+C to exit...\n";

    while (true) {
        // repeat
        // clear the screen and home cursor
        vector<unsigned char> input_data1 {0x1B, 0x5B, 0x6A};
        i2c.write(this_device.handle, input_data1, CLS_address);

        // display a message
        i2c.write(this_device.handle, "Temp: ", CLS_address);

        // read the temperature
        i2c_data output_data = i2c.read(this_device.handle, 2, TMP2_address);   // read 2 bytes
        int value = (int(output_data.data[0]) << 8) | int(output_data.data[1]);    // create integer from received bytes
        double result;
        if ((value >> 15) & 1 == 0) {
            result = value / 128.0;    // decode positive numbers
        }
        else {
            result = (value - 65535) / 128.0;   // decode negative numbers
        }

        // display the temperature
        ostringstream output;
        output.precision(2);
        output << fixed << result;
        i2c.write(this_device.handle, output.str(), CLS_address);

        // display a message
        vector<unsigned char> input_data2 {0};
        i2c.write(this_device.handle, input_data2, CLS_address);
        i2c.write(this_device.handle, "C", CLS_address);

        // delay 1s
        sleep(1000);
    }

    /* ----------------------------------------------------- */

    return;
}

/* ----------------------------------------------------- */

void signal_handler(int sig_num) {
    // reset the interface
    i2c.close(this_device.handle);

    // stop and reset the power supplies
    supplies_state.master_state = false;
    supplies.switch_(this_device.handle, supplies_state);
    supplies.close(this_device.handle);

    // close the connection
    device.close(this_device.handle);

    //cout << "\nPress Enter to exit...";
    //cin.get();
    exit(0);
    return;
}
