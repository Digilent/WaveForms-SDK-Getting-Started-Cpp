#include <chrono>           // needed for sleep
#include <signal.h>         // needed for keyboard interrupt
#include <vector>           // needed for data list handling
#include <cmath>            // needed for math functions
#include "dwf.h"
#include "device.h"         // needed for instrument control

#ifndef WF_TOOLS
#define WF_TOOLS

void ISR(int signum);
int device_handle = 0;

class Tools {
    private:
        class Window {
            public:
                const DwfWindow rectangula = DwfWindowRectangular;
                const DwfWindow triangular = DwfWindowTriangular;
                const DwfWindow hamming = DwfWindowHamming;
                const DwfWindow hann = DwfWindowHann;
                const DwfWindow cosine = DwfWindowCosine;
                const DwfWindow blackman_harris = DwfWindowBlackmanHarris;
                const DwfWindow flat_top = DwfWindowFlatTop;
                const DwfWindow kaiser = DwfWindowKaiser;
        };
    public:
        Window window;
        int get_time(void);
        void sleep(int millis);
        void keyboard_interrupt_reset(Device::Data device_data);
        template <typename T>
        inline T const& min(T const& a, T const& b);
        template <typename T>
        inline T const& max(T const& a, T const& b);
        std::vector<double> spectrum(std::vector<double> buffer, DwfWindow window, double sample_rate, double frequency_start, double frequency_stop);
} tools;

#endif
