/* WAVEFORM GENERATOR CONTROL FUNCTIONS: generate, close */

/* include the necessary libraries */
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

class Wavegen {
    private:
        class Function {
            /* function names */
            public:
                const FUNC custom = funcCustom;
                const FUNC sine = funcSine;
                const FUNC square = funcSquare;
                const FUNC triangle = funcTriangle;
                const FUNC noise = funcNoise;
                const FUNC dc = funcDC;
                const FUNC pulse = funcPulse;
                const FUNC trapezium = funcTrapezium;
                const FUNC sine_power = funcSinePower;
                const FUNC ramp_up = funcRampUp;
                const FUNC ramp_down = funcRampDown;
        };

    public:
        Function function;
        void generate(HDWF device_handle, int channel, FUNC function, double offset, double frequency = 1e03, double amplitude = 1, double symmetry = 50, double wait = 0, double run_time = 0, int repeat = 0, vector<double> data = vector<double>());
        void close(HDWF device_handle);
} wavegen;
