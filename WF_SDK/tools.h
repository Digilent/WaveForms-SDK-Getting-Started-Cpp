#include <chrono>           // needed for sleep
#include <signal.h>         // needed for keyboard interrupt
#include "dwf.h"
#include "device.h"         // needed for instrument control

#ifndef WF_TOOLS
#define WF_TOOLS

void ISR(int signum);
int device_handle = 0;

class Tools {
    public:
        int get_time(void);
        void sleep(int millis);
        void keyboard_interrupt_reset(Device::Data device_data);
} tools;

#endif
