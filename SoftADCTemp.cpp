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
  _currentInput = 0;
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
  switch (_currentState)
  {
    case STATE_IDLE: // start the discharge cycle
      digitalWrite(_dischargePin, HIGH); // discharge all capacitors
      _currentState = STATE_DISCHARGE;
      break;
    case STATE_DISCHARGE: // wait for the capacitors to discharge
      if (digitalRead(_inputPins[_currentInput]) == LOW) // capacitor is discharged
      {
        digitalWrite(_dischargePin, LOW); // stop discharging
        _startTimes[_currentInput] = micros(); // record the start time of the charging pulse
        _currentState = STATE_CHARGE;
      }
      break;
    case STATE_CHARGE: // wait for the capacitor to charge
      if (digitalRead(_inputPins[_currentInput]) == HIGH) // capacitor is charged
      {
        _readings[_currentInput] = micros() - _startTimes[_currentInput]; // record the duration of the charging pulse
        _currentInput++; // move to the next input
        if (_currentInput >= _numInputs) // all inputs are measured
        {
          _currentInput = 0; // reset the input index
          _currentState = STATE_IDLE; // go back to idle state
        }
        else // more inputs to measure
        {
          _currentState = STATE_DISCHARGE; // start the next discharge cycle
        }
      }
      break;
  }
}

bool SoftwareADCTemp::available()
{
  return (_currentState == STATE_IDLE); // readings are available when in idle state
}

uint32_t* SoftwareADCTemp::getReadings()
{
  return _readings; // return the raw ADC readings array
}
