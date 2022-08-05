/* WAVEFORM GENERATOR CONTROL FUNCTIONS: generate, close, enable, disable */

/* include the necessary libraries */
#include <vector>
#include "dwf.h"
#include "device.h"

#ifndef WF_WAVEGEN
#define WF_WAVEGEN

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

        class State {
            public:
                bool on = false;
                bool off = true;
                std::vector<bool> channel{false, false};
                State& operator=(const State &data) {
                    if (this != &data) {
                        on = data.on;
                        off = data.off;
                        channel = data.channel;
                    }
                    return *this;
                }
        };

    public:
        Function function;
        State state;
        void generate(Device::Data device_data, int channel, FUNC function, double offset, double frequency = 1e03, double amplitude = 1, double symmetry = 50, double wait = 0, double run_time = 0, int repeat = 0, std::vector<double> data = std::vector<double>());
        void close(Device::Data device_data, int channel = 0);
        void enable(Device::Data device_data, int channel);
        void disable(Device::Data device_data, int channel);
} wavegen;

#endif
