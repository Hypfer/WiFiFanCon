#include <Arduino.h>
#include <esp32-hal.h>
#include <pins_arduino.h>

static volatile int counter_rpm1 = 0;
static volatile int counter_rpm2 = 0;
static volatile int counter_rpm3 = 0;
static volatile int counter_rpm4 = 0;

int last_rpm1 = 0;
int last_rpm2 = 0;
int last_rpm3 = 0;
int last_rpm4 = 0;

unsigned long millisecondsLastTachoMeasurement = 0;

void IRAM_ATTR rpm_fan1() {
  counter_rpm1++;
}

void IRAM_ATTR rpm_fan2() {
  counter_rpm2++;
}

void IRAM_ATTR rpm_fan3() {
  counter_rpm3++;
}

void IRAM_ATTR rpm_fan4() {
  counter_rpm4++;
}

void initTacho(void) {
  pinMode(tachoPin1, INPUT);
  pinMode(tachoPin2, INPUT);
  pinMode(tachoPin3, INPUT);
  pinMode(tachoPin4, INPUT);

  digitalWrite(tachoPin1, HIGH);
  digitalWrite(tachoPin2, HIGH);
  digitalWrite(tachoPin3, HIGH);
  digitalWrite(tachoPin4, HIGH);

  attachInterrupt(digitalPinToInterrupt(tachoPin1), rpm_fan1, FALLING);
  attachInterrupt(digitalPinToInterrupt(tachoPin2), rpm_fan2, FALLING);
  attachInterrupt(digitalPinToInterrupt(tachoPin3), rpm_fan3, FALLING);
  attachInterrupt(digitalPinToInterrupt(tachoPin4), rpm_fan4, FALLING);



  Serial.println("Tacho measurement initialized");
}

void updateTacho(void) {
  // start of tacho measurement
  if ((unsigned long)(millis() - millisecondsLastTachoMeasurement) >= tachoUpdateCycle)
  {
    // detach interrupts while calculating rpm
    detachInterrupt(digitalPinToInterrupt(tachoPin1));
    detachInterrupt(digitalPinToInterrupt(tachoPin2));
    detachInterrupt(digitalPinToInterrupt(tachoPin3));
    detachInterrupt(digitalPinToInterrupt(tachoPin4));

    // calculate rpm
    last_rpm1 = counter_rpm1 * ((float)60 / (float)numberOfInterruptsInOneSingleRotation) * ((float)1000 / (float)tachoUpdateCycle);
    last_rpm2 = counter_rpm2 * ((float)60 / (float)numberOfInterruptsInOneSingleRotation) * ((float)1000 / (float)tachoUpdateCycle);
    last_rpm3 = counter_rpm3 * ((float)60 / (float)numberOfInterruptsInOneSingleRotation) * ((float)1000 / (float)tachoUpdateCycle);
    last_rpm4 = counter_rpm4 * ((float)60 / (float)numberOfInterruptsInOneSingleRotation) * ((float)1000 / (float)tachoUpdateCycle);

    
    //Serial.printf("fan rpm = %d\r\n", last_rpm);

    // reset counter
    counter_rpm1 = 0;
    counter_rpm2 = 0;
    counter_rpm3 = 0;
    counter_rpm4 = 0;

    // store milliseconds when tacho was measured the last time
    millisecondsLastTachoMeasurement = millis();

    // attach interrupts again
    attachInterrupt(digitalPinToInterrupt(tachoPin1), rpm_fan1, FALLING);
    attachInterrupt(digitalPinToInterrupt(tachoPin2), rpm_fan2, FALLING);
    attachInterrupt(digitalPinToInterrupt(tachoPin3), rpm_fan3, FALLING);
    attachInterrupt(digitalPinToInterrupt(tachoPin4), rpm_fan4, FALLING);
  }
}

int getTacho1() {
  return last_rpm1;
}

int getTacho2() {
  return last_rpm2;
}

int getTacho3() {
  return last_rpm3;
}

int getTacho4() {
  return last_rpm4;
}
