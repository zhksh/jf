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
#define JOYSTICK0  A0
#define JOYSTICK1  A1


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
  Serial.println("Sender setup");
  mySwitch.enableTransmit(TRANSMITTER_PIN);
  
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(JOYSTICK0, INPUT);
  pinMode(JOYSTICK1, INPUT);
}

void loop() {
  int schalter1 = digitalRead(SWITCH1);
  int schalter2 = digitalRead(SWITCH2);
  int schalter3 = digitalRead(SWITCH3);
  int joystick0 = analogRead(JOYSTICK0);
  int joystick1 = analogRead(JOYSTICK1);
  n = schalter1 + 2 * schalter2 + 4 * schalter3 + 8 * joystick0 + 8192*joystick1;
  


  transmit(n);
  
  
  Serial.println("werfoiuhhahahaha");
  delay(500);
}

