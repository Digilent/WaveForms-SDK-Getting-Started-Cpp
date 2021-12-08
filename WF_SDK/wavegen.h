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

class Function {
    /* function names */
    FUNC custom = funcCustom;
    FUNC sine = funcSine;
    FUNC square = funcSquare;
    FUNC triangle = funcTriangle;
    FUNC noise = funcNoise;
    FUNC dc = funcDC;
    FUNC pulse = funcPulse;
    FUNC trapezium = funcTrapezium;
    FUNC sine_power = funcSinePower;
    FUNC ramp_up = funcRampUp;
    FUNC ramp_down = funcRampDown;
} function;

/* ----------------------------------------------------- */

class Wavegen {
    public:
        void generate(HDWF device_handle, int channel, FUNC function, double offset, double frequency = 1e03, double amplitude = 1, double symmetry = 50, double wait = 0, double run_time = 0, int repeat = 0, double* data = nullptr);
        void close(HDWF device_handle);
} wavegen;
