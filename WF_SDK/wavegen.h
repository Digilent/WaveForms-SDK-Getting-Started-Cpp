/* WAVEFORM GENERATOR CONTROL FUNCTIONS: generate, close */

/* include the necessary libraries */

/* include the constants and the WaveForms function library */
#ifdef WIN32
#include "C:/Program Files (x86)/Digilent/WaveFormsSDK/inc/dwf.h"
#elif __APPLE__
#include "/Library/Frameworks/dwf.framework/Headers/dwf.h"
#else
#include <digilent/waveforms/dwf.h>
#endif

/* ----------------------------------------------------- */

class Wavegen {
    public:
        class function {
            /* function names */
            public:
                static const FUNC custom = funcCustom;
                static const FUNC sine = funcSine;
                static const FUNC square = funcSquare;
                static const FUNC triangle = funcTriangle;
                static const FUNC noise = funcNoise;
                static const FUNC dc = funcDC;
                static const FUNC pulse = funcPulse;
                static const FUNC trapezium = funcTrapezium;
                static const FUNC sine_power = funcSinePower;
                static const FUNC ramp_up = funcRampUp;
                static const FUNC ramp_down = funcRampDown;
        };

        void generate(HDWF device_handle, int channel, FUNC function, double offset, double frequency = 1e03, double amplitude = 1, double symmetry = 50, double wait = 0, double run_time = 0, int repeat = 0, double* data = nullptr);
        void close(HDWF device_handle);
} wavegen;
