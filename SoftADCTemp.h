// SoftwareADCTemp.h

#ifndef SoftwareADCTemp_h
#define SoftwareADCTemp_h

#include "Arduino.h"

class SoftwareADCTemp
{
  public:
    SoftwareADCTemp(int dischargePin, int numInputs, int* inputPins); // constructor
    void begin(); // initialize the library
    void update(); // update the readings
    bool available(); // check if readings are available
    uint32_t* getReadings(); // get the raw ADC readings
  private:
    int _dischargePin; // pin used to discharge all capacitors
    int _numInputs; // number of inputs for thermistors
    int* _inputPins; // array of pins for thermistors
    uint32_t* _readings; // array of raw ADC readings
    uint32_t* _startTimes; // array of start times for charging pulses
    uint8_t _currentState; // current state of the state machine
    uint8_t _currentInput; // current input being measured
};

#endif
