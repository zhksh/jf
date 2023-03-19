#include <Arduino.h>

#include <RCSwitch.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PREFIX 0xfe000000
#define MSG_LENGTH 32


// PINS
#define TRANSMITTER_PIN 10
#define SWITCH1  2
#define SWITCH2  3
#define SWITCH3  4

#define TEMPSENSOR0  4
#define TEMPSENSOR1  5

#define SWITCH3  
#define JOYSTICK0  A0
#define JOYSTICK1  A1

#define TESENSOR A0


RCSwitch mySwitch = RCSwitch();
long n;

unsigned long encode(long code) {
  long value = code | PREFIX;
  return value;
  // return code;
}


void transmit(int code) {

  long msg = encode(code);
  Serial.print("code:");
  Serial.print(code);
  Serial.print(", encoded:");
  Serial.println(msg);
  mySwitch.send(msg, MSG_LENGTH);
}


void setup() {
  Serial.begin(9600);
  // Serial.println("Sender setup");
  mySwitch.enableTransmit(TRANSMITTER_PIN);
  mySwitch.setProtocol(2);

 // Optional set number of transmission repetitions.
  mySwitch.setRepeatTransmit(15);
  
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(JOYSTICK0, INPUT);
  pinMode(JOYSTICK1, INPUT);
}

void printToSerial(float tb, float temp0, float temp1, float aux){
  Serial.print("tb");
  Serial.print(":");
  Serial.print(tb);
  Serial.print(";");

  Serial.print("tmp0");
  Serial.print(":");
  Serial.print(temp0);
  Serial.print(";");

  Serial.print("tmp1");
  Serial.print(":");
  Serial.print(temp1);
  Serial.print(";");

  Serial.print("aux");
  Serial.print(":");
  Serial.print(aux);

  Serial.println("");


}

void loop() {

  // float sensorValue = digitalRead(TESENSOR);
  float tbval = analogRead(TESENSOR);
  float temp0val = digitalRead(TEMPSENSOR0);
  float temp1val = digitalRead(TEMPSENSOR1);



  // sensorValue/=350;
  // sensorValue*=100;
  // sensorValue=100-sensorValue;
  printToSerial(tbval, temp0val, temp1val, 0.0);


  delay(100);
  // int schalter1 = digitalRead(SWITCH1);
  // int schalter2 = digitalRead(SWITCH2);
  // int schalter3 = digitalRead(SWITCH3);
  // int joystick0 = analogRead(JOYSTICK0);
  // int joystick1 = analogRead(JOYSTICK1);
  // // n = schalter1 + 2 * schalter2 + 4 * schalter3 + 8 * joystick0 + 8192*joystick1;
  // transmit(n++);    
  // delay(5);
}

