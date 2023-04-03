// optimized_example.ino

#include "SoftwareADCTemp.h"

#define NUM_INPUTS 4 // number of thermistors
#define DISCHARGE_PIN 12 // pin used to discharge capacitors
#define INTERVAL 1000 // interval in milliseconds to update readings

int inputPins[NUM_INPUTS] = {2, 3, 4, 5}; // pins for thermistors
uint32_t readings[NUM_INPUTS]; // array to store readings
SoftwareADCTemp adcTemp(DISCHARGE_PIN, NUM_INPUTS, inputPins); // create an instance of the library

void setup()
{
  Serial.begin(9600); // initialize serial communication
  adcTemp.begin(); // initialize the library
  Timer1.initialize(INTERVAL * 1000); // initialize timer1 with the interval in microseconds
  Timer1.attachInterrupt(updateReadings); // attach the updateReadings function to timer1 interrupt
}

void loop()
{
  // do nothing in the main loop
}

void updateReadings()
{
  static uint32_t lastTime = 0; // variable to store the last time of the interrupt
  uint32_t currentTime = micros(); // get the current time of the interrupt
  uint32_t deltaTime = currentTime - lastTime; // calculate the delta time between interrupts
  lastTime = currentTime; // update the last time variable
  adcTemp.update(); // update the readings from the library
  if (adcTemp.available()) // check if readings are available
  {
    uint32_t* rawReadings = adcTemp.getReadings(); // get the raw ADC readings from the library
    for (int i = 0; i < NUM_INPUTS; i++)
    {
      readings[i] = rawReadings[i] * INTERVAL * 1000 / deltaTime; // correct the readings for interrupt jitter by scaling them with the ratio of expected and actual delta time
      Serial.print("Input ");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(readings[i]); // print the readings to serial monitor
    }
    Serial.println();
  }
}
