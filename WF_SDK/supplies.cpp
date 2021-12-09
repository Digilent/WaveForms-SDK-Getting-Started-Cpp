/* POWER SUPPLIES CONTROL FUNCTIONS: switch, switch_fixed, switch_variable, switch_digital, switch_6V, switch_25V, close */

/* include the header */
#include "supplies.h"

/* ----------------------------------------------------- */

void Supplies::switch_fixed(HDWF device_handle, bool master_state, bool positive_state, bool negative_state) {
    /*
        turn the power supplies on/off

        parameters: - device handle
                    - master switch - true = on, false = off
                    - positive supply switch - true = on, false = off
                    - negative supply switch - true = on, false = off
    */
    // enable/disable the positive supply
    FDwfAnalogIOChannelNodeSet(device_handle, 0, 0, int(positive_state));
    
    // enable the negative supply
    FDwfAnalogIOChannelNodeSet(device_handle, 1, 0, int(negative_state));
    
    // start/stop the supplies - master switch
    FDwfAnalogIOEnableSet(device_handle, int(master_state));
    return;
}

/* ----------------------------------------------------- */

void Supplies::switch_variable(HDWF device_handle, bool master_state, bool positive_state, bool negative_state, double positive_voltage, double negative_voltage) {
    /*
        turn the power supplies on/off

        parameters: - device handle
                    - master switch - true = on, false = off
                    - positive supply switch - true = on, false = off
                    - negative supply switch - true = on, false = off
                    - positive supply voltage in Volts
                    - negative supply voltage in Volts
    */
    // set positive voltage
    positive_voltage = max(0.0, min(5.0, positive_voltage));
    FDwfAnalogIOChannelNodeSet(device_handle, 0, 1, positive_voltage);
    
    // set negative voltage
    negative_voltage *= -1;
    FDwfAnalogIOChannelNodeSet(device_handle, 1, 1, negative_voltage);

    // enable/disable the positive supply
    FDwfAnalogIOChannelNodeSet(device_handle, 0, 0, int(positive_state));
    
    // enable the negative supply
    FDwfAnalogIOChannelNodeSet(device_handle, 1, 0, int(negative_state));
    
    // start/stop the supplies - master switch
    FDwfAnalogIOEnableSet(device_handle, int(master_state));
    return;
}

/* ----------------------------------------------------- */

void Supplies::switch_digital(HDWF device_handle, bool master_state, double voltage) {
    /*
        turn the power supplies on/off

        parameters: - device handle
                    - master switch - true = on, false = off
                    - supply voltage in Volts
    */
    // set supply voltage
    voltage = max(1.2, min(3.3, voltage));
    FDwfAnalogIOChannelNodeSet(device_handle, 0, 0, voltage);
    
    // start/stop the supplies - master switch
    FDwfAnalogIOEnableSet(device_handle, int(master_state));
    return;
}

/* ----------------------------------------------------- */

void Supplies::switch_6V(HDWF device_handle, bool master_state, double voltage, double current) {
    /*
        turn the 6V supply on the ADP5250 on/off

        parameters: - master switch - true = on, false = off
                    - voltage in volts between 0-6
                    - current in amperes between 0-1
    */
    // set the voltage
    voltage = max(0.0, min(6.0, voltage));
    FDwfAnalogIOChannelNodeSet(device_handle, 0, 1, voltage);
    
    // set the current
    current = max(0.0, min(1.0, current));
    FDwfAnalogIOChannelNodeSet(device_handle, 0, 2, current);
    
    // start/stop the supply - master switch
    FDwfAnalogIOChannelNodeSet(device_handle, 0, 0, double(master_state));
    FDwfAnalogIOEnableSet(device_handle, int(master_state));
    return;
}

/* ----------------------------------------------------- */

void Supplies::switch_25V(HDWF device_handle, bool positive_state, bool negative_state, double positive_voltage, double negative_voltage, double positive_current, double negative_current) {
    /*
        turn the 25V power supplies on/off on the ADP5250

        parameters: - positive supply switch - true = on, false = off
                    - negative supply switch - true = on, false = off
                    - positive supply voltage in Volts
                    - negative supply voltage in Volts
                    - positive supply current limit
                    - negative supply current limit
    */
    // set positive voltage
    positive_voltage = max(0.0, min(25.0, positive_voltage));
    FDwfAnalogIOChannelNodeSet(device_handle, 1, 1, positive_voltage);
    
    // set negative voltage
    negative_voltage *= -1;
    FDwfAnalogIOChannelNodeSet(device_handle, 2, 1, negative_voltage);

    // set positive current limit
    positive_current = max(0.0, min(0.5, positive_current));
    FDwfAnalogIOChannelNodeSet(device_handle, 1, 2, positive_current);
    
    // set negative current limit
    negative_current *= -1;
    FDwfAnalogIOChannelNodeSet(device_handle, 2, 2, negative_current);

    // enable/disable the supplies
    FDwfAnalogIOChannelNodeSet(device_handle, 1, 0, double(positive_state));
    FDwfAnalogIOChannelNodeSet(device_handle, 2, 0, double(negative_state));
    
    // master switch
    FDwfAnalogIOEnableSet(device_handle, int(positive_state | negative_state));
    return;
}

/* ----------------------------------------------------- */

bool Supplies::switch_(HDWF device_handle, supplies_data state) {
    /*
        turn the power supplies on/off

        parameters: - device handle
                    - class containing supplies data:
                        - device_name
                        - master_state
                        - state and/or positive_state and negative_state
                        - voltage and/or positive_voltage and negative_voltage
                        - current and/or positive_current and negative_current

        returns:    - true on success, false on error
    */
    if (state.name == "Analog Discovery") {
        // switch fixed supplies on AD
        if (state.negative_state >= 0 && state.positive_state >= 0) {
            // both supplies
            switch_fixed(device_handle, state.master_state, state.positive_state, state.negative_state);
            return true;
        }
        else if (state.state >= 0) {
            // positive supply only
            switch_fixed(device_handle, state.master_state, state.state, false);
            return true;
        }
        else {
            // error in input data
            return false;
        }
    }

    else if (state.name == "Analog Discovery 2" || state.name == "Analog Discovery Studio") {
        // switch variable supplies on AD2
        if (state.negative_state >= 0 && state.positive_state >= 0) {
            // switch both supplies
            switch_variable(device_handle, state.master_state, state.positive_state, state.negative_state, state.positive_voltage, state.negative_voltage);
            return true;
        }
        else if (state.state >= 0) {
            // switch only the positive supply
            switch_variable(device_handle, state.master_state, state.state, false, state.voltage, 0);
            return true;
        }
        else {
            // error in input data
            return false;
        }
    }

    else if (state.name == "Digital Discovery" || state.name == "Analog Discovery Pro 3X50") {
        // switch the digital supply on DD, or ADP3x50
        if (state.master_state == true) {
            switch_digital(device_handle, state.state, state.voltage);
            return true;
        }
        else if (state.master_state == false) {
            switch_digital(device_handle, false, 3.3);
            return true;
        }
        else {
            return false;
        }
    }

    else if (state.name == "Analog Discovery Pro 5250") {
        bool error_flag = false;

        // switch the 6V supply on ADP5250
        if (state.master_state == true) {
            if (state.state >= 0 && state.current >= 0) {
                // try to limit the current
                switch_6V(device_handle, state.state, state.voltage, state.current);
            }
            else if (state.state >= 0) {
                // try without current limitation
                switch_6V(device_handle, state.state, state.voltage);
            }
            else {
                // no data for the 6V supply
                error_flag = true;
            }

            if (state.positive_state >= 0 && state.negative_state >= 0 && state.positive_current >= 0 && state.negative_current <= 0) {
                // switch both supplies and limit current
                switch_25V(device_handle, state.positive_state, state.negative_state, state.positive_voltage, state.negative_voltage, state.positive_current, state.negative_current);
            }
            else if (state.positive_state >= 0 && state.negative_state >= 0) {
                // switch both suplpies without current limitation
                switch_25V(device_handle, state.positive_state, state.negative_state, state.positive_voltage, state.negative_voltage);
            }
            else if (error_flag == false) {
                // data only for the 6V supply
                return true;
            }
            else if (error_flag == true) {
                // invalid input data
                return false;
            }
        }
        else if (state.master_state == false) {
            // turn everything off
            switch_6V(device_handle, false, 0, 1);
            switch_25V(device_handle, false, false, 0, 0, 0.5, -0.5);
            return true;
        }
    }

    // no such device
    return false;
}

/* ----------------------------------------------------- */

void Supplies::close(HDWF device_handle) {
    /*
        reset the supplies
    */
    FDwfAnalogIOReset(device_handle);
    return;
}
