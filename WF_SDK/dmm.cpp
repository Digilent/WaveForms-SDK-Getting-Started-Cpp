/* DIGITAL MULTIMETER CONTROL FUNCTIONS: open, measure, close */

/* include the header */
#include "dmm.h"

/* ----------------------------------------------------- */

void DMM::open(HDWF device_handle) {
    /*
        initialize the digital multimeter
    */
    // enable the DMM
    FDwfAnalogIOChannelNodeSet(device_handle, 3, 0, double(1.0));
    return;
}

/* ----------------------------------------------------- */

double DMM::measure(HDWF device_handle, string mode, bool ac, double range, bool high_impedance) {
    /*
        measure a voltage/current/resistance/continuity/temperature

        parameters: - device handler
                    - mode: "voltage", "low current", "high current", "resistance", "continuity", "diode", "temperature"
                    - ac: True means AC value, False means DC value, default is DC
                    - range: voltage/current/resistance/temperature range, 0 means auto, default is auto
                    - high_impedance: input impedance for DC voltage measurement, False means 10MΩ, True means 10GΩ, default is 10MΩ
        
        returns:    - the measured value in V/A/Ω/°C, or -1 on error
    */
    // set voltage mode
    if (mode == "voltage") {
        // set coupling
        if (ac == true) {
            FDwfAnalogIOChannelNodeSet(device_handle, 3, 1, DwfDmmACVoltage);
        }
        else {
            FDwfAnalogIOChannelNodeSet(device_handle, 3, 1, DwfDmmDCVoltage);
        }

        // set input impedance
        if (high_impedance == true) {
            FDwfAnalogIOChannelNodeSet(device_handle, 3, 5, double(1.0));
        }
        else {
            FDwfAnalogIOChannelNodeSet(device_handle, 3, 5,double(0.0));
        }
    }

    // set high current mode
    else if (mode == "high current") {
        // set coupling
        if (ac == true) {
            FDwfAnalogIOChannelNodeSet(device_handle, 3, 1, DwfDmmACCurrent);
        }
        else {
            FDwfAnalogIOChannelNodeSet(device_handle, 3, 1, DwfDmmDCCurrent);
        }
    }

    // set low current mode
    else if (mode == "low current") {
        // set coupling
        if (ac == true) {
            FDwfAnalogIOChannelNodeSet(device_handle, 3, 1, DwfDmmACLowCurrent);
        }
        else {
            FDwfAnalogIOChannelNodeSet(device_handle, 3, 1, DwfDmmDCLowCurrent);
        }
    }
            
    // set resistance mode
    else if (mode == "resistance") {
        FDwfAnalogIOChannelNodeSet(device_handle, 3, 1, DwfDmmResistance);
    }

    // set continuity mode
    else if (mode == "continuity") {
        FDwfAnalogIOChannelNodeSet(device_handle, 3, 1, DwfDmmContinuity);
    }

    // set diode mode
    else if (mode == "diode") {
        FDwfAnalogIOChannelNodeSet(device_handle, 3, 1, DwfDmmDiode);
    }

    // set temperature mode
    else if (mode == "temperature") {
        FDwfAnalogIOChannelNodeSet(device_handle, 3, 1, DwfDmmTemperature);
    }
        
    // set range
    FDwfAnalogIOChannelNodeSet(device_handle, 3, 2, range);

    // fetch analog I/O status
    if (FDwfAnalogIOStatus(device_handle) == 0) {
        // signal error
        return double(-1.0);
    }

    // get reading
    double measurement = 0;
    FDwfAnalogIOChannelNodeStatus(device_handle, 3, 3, &measurement);
    return measurement;
}

/* ----------------------------------------------------- */

void DMM::close(HDWF device_handle) {
    /*
        reset the instrument
    */
    // disable the DMM
    FDwfAnalogIOChannelNodeSet(device_handle, 3, 0, double(0.0));
    // reset the instrument
    FDwfAnalogIOReset(device_handle);
    return;
}
