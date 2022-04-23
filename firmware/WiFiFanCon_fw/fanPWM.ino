#include <Arduino.h>
#include <esp32-hal.h>
#include <esp32-hal-ledc.h>

int dutyCycle1 = 0;
int dutyCycle2 = 0;
int dutyCycle3 = 0;
int dutyCycle4 = 0;

void initPWM(void){
  ledcSetup(0, pwmFreq, pwmResolution);
  ledcAttachPin(pwmPin1, 0); //0 = PWM channel
  ledcSetup(1, pwmFreq, pwmResolution);
  ledcAttachPin(pwmPin2, 1);
  ledcSetup(2, pwmFreq, pwmResolution);
  ledcAttachPin(pwmPin3, 2);
  ledcSetup(3, pwmFreq, pwmResolution);
  ledcAttachPin(pwmPin4, 3);


  setDutyCycle1(100);
  delay(500);
  setDutyCycle2(100);
  delay(500);
  setDutyCycle3(100);
  delay(500);
  setDutyCycle4(100);
  delay(500);

  setDutyCycle1(initialDutyCycle);
  setDutyCycle2(initialDutyCycle);
  setDutyCycle3(initialDutyCycle);
  setDutyCycle4(initialDutyCycle);


  Serial.println("Fan PWM sucessfully initialized");
}

void setDutyCycle1(int percent){
  dutyCycle1 = setDutyCycle(0, percent);
}

void setDutyCycle2(int percent){
  dutyCycle2 = setDutyCycle(1, percent);
}

void setDutyCycle3(int percent){
  dutyCycle3 = setDutyCycle(2, percent);
}

void setDutyCycle4(int percent){
  dutyCycle4 = setDutyCycle(3, percent);
}

int setDutyCycle(int channel, int percent) {
  int value = constrain(percent, 0, 100);
  
  ledcWrite(channel, map(value, 0, 100, 0, 255));

  return value;
}


int getDutyCycle1(){
  return dutyCycle1;
}

int getDutyCycle2() {
  return dutyCycle2;
}

int getDutyCycle3() {
  return dutyCycle3;
}

int getDutyCycle4() {
  return dutyCycle4;
}
