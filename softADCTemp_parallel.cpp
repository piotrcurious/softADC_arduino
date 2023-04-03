```markdown
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
    uint8_t _numMeasured; // number of inputs measured in the current cycle
    uint32_t _lastTime; // last time of the update function call
};

#endif

// SoftwareADCTemp.cpp

#include "SoftwareADCTemp.h"

#define STATE_IDLE 0 // idle state
#define STATE_DISCHARGE 1 // discharge state
#define STATE_CHARGE 2 // charge state

SoftwareADCTemp::SoftwareADCTemp(int dischargePin, int numInputs, int* inputPins)
{
  _dischargePin = dischargePin;
  _numInputs = numInputs;
  _inputPins = inputPins;
  _readings = new uint32_t[numInputs];
  _startTimes = new uint32_t[numInputs];
  _currentState = STATE_IDLE;
  _numMeasured = 0;
  _lastTime = 0;
}

void SoftwareADCTemp::begin()
{
  pinMode(_dischargePin, OUTPUT);
  digitalWrite(_dischargePin, LOW);
  for (int i = 0; i < _numInputs; i++)
  {
    pinMode(_inputPins[i], INPUT);
    _readings[i] = 0;
    _startTimes[i] = 0;
  }
}

void SoftwareADCTemp::update()
{
  uint32_t currentTime = micros(); // get the current time of the update function call
  switch (_currentState)
  {
    case STATE_IDLE: // start the discharge cycle
      digitalWrite(_dischargePin, HIGH); // discharge all capacitors
      _currentState = STATE_DISCHARGE;
      break;
    case STATE_DISCHARGE: // wait for the capacitors to discharge
      bool allDischarged = true; // flag to check if all capacitors are discharged
      for (int i = 0; i < _numInputs; i++)
      {
        if (digitalRead(_inputPins[i]) == HIGH) // capacitor is not discharged yet
        {
          allDischarged = false; // set the flag to false
          break; // exit the loop
        }
      }
      if (allDischarged) // all capacitors are discharged
      {
        digitalWrite(_dischargePin, LOW); // stop discharging
        for (int i = 0; i < _numInputs; i++)
        {
          _startTimes[i] = currentTime; // record the start time of the charging pulse as the current time of the update function call to correct for interrupt granularity for all inputs in parallel
        }
        _currentState = STATE_CHARGE;
      }
      break;
    case STATE_CHARGE: // wait for the capacitors to charge
      for (int i = 0; i < _numInputs; i++)
      {
        if (digitalRead(_inputPins[i]) == HIGH) // capacitor is charged
        {
          if (_startTimes[i] != 0) // start time is not zero, meaning reading is not recorded yet
          {
            _readings[i] = currentTime - _startTimes[i]; // record the duration of the charging pulse as the difference between the current time and the start time of the update function call to correct for interrupt granularity for each input individually
            _startTimes[i] = 0; // set the start time to zero to mark that reading is recorded 
            _numMeasured++; // increment the number of inputs measured in the current cycle
          }
        }
      }
      if (_numMeasured >= _numInputs) // all inputs are measured in the current cycle
