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
int CLS_cs = 0;
int ALS_cs = 1;
int sck = 2;
int miso = 3;
int mosi = 4;

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

    // initialize the spi interface on DIO0, DIO1, DIO2, DIO3 and DIO4
    spi.open(this_device.handle, CLS_cs, sck, miso, mosi);
    spi.open(this_device.handle, ALS_cs, sck, miso, mosi);

    signal(SIGINT, signal_handler);

    cout << "Press Ctrl+C to exit...\n";

    while (true) {
        // repeat
        // clear the screen and home cursor
        spi.write(this_device.handle, "\x1b[j", CLS_cs);

        // display a message
        spi.write(this_device.handle, "Lum: ", CLS_cs);

        // read the temperature
        vector<unsigned char> message = spi.read(this_device.handle, 2, ALS_cs);
        double value = ((int(message[0]) << 3) | (int(message[1]) >> 4)) / 1.27;

        // display the temperature
        ostringstream output;
        output.precision(2);
        output << fixed << value;
        spi.write(this_device.handle, output.str(), CLS_cs);

        // display a message
        spi.write(this_device.handle, "%", CLS_cs);

        // delay 1s
        sleep(1000);
    }

    /* ----------------------------------------------------- */

    return;
}

/* ----------------------------------------------------- */

void signal_handler(int sig_num) {
    // reset the interface
    spi.close(this_device.handle);

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
