/* PATTERN GENERATOR CONTROL FUNCTIONS: generate, close */

/* include the necessary libraries */
#include <math.h>

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

class Pattern {
    public:
        class function {
            /* function names */
            public:
                static const DwfDigitalOutType pulse = DwfDigitalOutTypePulse;
                static const DwfDigitalOutType custom = DwfDigitalOutTypeCustom;
                static const DwfDigitalOutType random = DwfDigitalOutTypeRandom;
        };

        class trigger_source {
            /* trigger source names */
            public:
                static const TRIGSRC none = trigsrcNone;
                static const TRIGSRC analog = trigsrcDetectorAnalogIn;
                static const TRIGSRC digital = trigsrcDetectorDigitalIn;
                static constexpr TRIGSRC external[5] = {trigsrcNone, trigsrcExternal1, trigsrcExternal2, trigsrcExternal3, trigsrcExternal4};
            };

        void generate(HDWF device_handle, int channel, DwfDigitalOutType function, double frequency, double duty_cycle = 50.0, unsigned char* data = nullptr, double wait = 0, int repeat = 0, bool trigger_enabled = false, TRIGSRC trigger_source = trigger_source::none, bool trigger_edge_rising = true);
        void close(HDWF device_handle);
} pattern;
