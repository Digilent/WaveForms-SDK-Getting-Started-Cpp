## Description
Demo package for the WaveForms SDK Getting Started guide and multiple test scripts for different instruments.

Check: [Getting Started with the WaveForms SDK](https://digilent.com/reference/test-and-measurement/guides/waveforms-sdk-getting-started) for more details.

***

## Available tests:
* empty test template
* analog signal generation and recording test
* digital signal generation and recording test
* blinking LEDs with the Suplpies and the Static I/O instruments
* UART in/out test using the Pmod CLS and the Pmod MAXSonar
* SPI in/out test using the Pmod CLS and the Pmod ALS
* I2C in/out test using the Pmod CLS and the Pmod TMP2
* board temperature test
* device information logging

***

## Available instruments and functions:
### Device
* open
* check_error
* close
* temperature

### Oscilloscope
* open
* measure
* trigger
* record
* close

### Waveform Generator
* generate
* close
* enable
* disable

### Power Supplies
* switch_
* close

### Digital Multimeter
* open
* measure
* close

### Logic Analyzer
* open
* trigger
* record
* close

### Pattern Generator
* generate
* close
* enable
* disable

### Static I/O
* set_mode
* get_state
* set_state
* set_current
* set_pull
* close

### Protocol
#### UART
* open
* read
* write
* close

#### SPI
* open
* read
* write
* exchange
* close

#### I2C
* open
* read
* write
* exchange
* close
