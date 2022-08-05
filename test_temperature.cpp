#include "WF_SDK/WF_SDK.h"  // include all classes and functions
#include <iostream>         // needed for input/output
#include <signal.h>         // needed for exiting the infinite loop
#include <cmath>            // needed for ceil

//using namespace std;

// macros
#if (defined(_WIN32) || defined(_WIN64))
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
Device::Data device_data;

/* ----------------------------------------------------- */

int main(void) {
    // connect to the device
    device_data = device.open();

    // check for connection errors
    device.check_error(device_data);

    /* ----------------------------------------------------- */

    signal(SIGINT, signal_handler);

    std::cout << "Press Ctrl+C to exit..." << std::endl;

    while (true) {
        double temp = device.temperature(device_data);  // get board temperature
        std::cout << "board temperature: " << std::ceil(temp * 100.0) / 100.0 << "C" << std::endl;  // display temperature
        sleep(500);
    }

    return 0;
}

/* ----------------------------------------------------- */

void signal_handler(int sig_num) {
    // close the connection
    device.close(device_data);
    exit(0);
    return;
}
