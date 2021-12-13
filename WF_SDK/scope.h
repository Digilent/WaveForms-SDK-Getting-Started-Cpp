/* OSCILLOSCOPE CONTROL FUNCTIONS: open, measure, trigger, record, close */

/* include the necessary libraries */

/* include the constants and the WaveForms function library */
#ifdef _WIN32
#include "C:/Program Files (x86)/Digilent/WaveFormsSDK/inc/dwf.h"
#elif __APPLE__
#include "/Library/Frameworks/dwf.framework/Headers/dwf.h"
#else
#include <digilent/waveforms/dwf.h>
#endif

/* ----------------------------------------------------- */

struct Scope_Data {
    double* buffer;
    double* time;
};

typedef struct Scope_Data scope_data;

/* ----------------------------------------------------- */



/* ----------------------------------------------------- */

class Scope {
    private:
        class Trigger_Source {
            /* trigger source names */
            public:
                const TRIGSRC none = trigsrcNone;
                const TRIGSRC analog = trigsrcDetectorAnalogIn;
                const TRIGSRC digital = trigsrcDetectorDigitalIn;
                const TRIGSRC external[5] = {trigsrcNone, trigsrcExternal1, trigsrcExternal2, trigsrcExternal3, trigsrcExternal4};
        };

    public:
        static Trigger_Source trigger_source;
        void open(HDWF device_handle, double sampling_frequency = 20e06, int buffer_size = 8192, double offset = 0, double amplitude_range = 5);
        double measure(HDWF device_handle, int channel);
        void trigger(HDWF device_handle, bool enable, const TRIGSRC source = trigger_source.none, int channel = 1, double timeout = 0, bool edge_rising = true, double level = 0);
        scope_data record(HDWF device_handle, int channel, double sampling_frequency=20e06, int buffer_size=8192);
        void close(HDWF device_handle);
} scope;
