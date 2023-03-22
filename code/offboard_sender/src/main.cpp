#include <RCSwitch.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PREFIX 0xff000000
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
  long schalter1 = digitalRead(SWITCH1);
  long schalter2 = digitalRead(SWITCH2);
  long schalter3 = digitalRead(SWITCH3);
  long joystick0 = analogRead(JOYSTICK0);
  long joystick1 = analogRead(JOYSTICK1);
  n = schalter1 + 2 * schalter2 + 4 * schalter3 + 8 * joystick0 + 8192 * joystick1;
  Serial.print(schalter1);
  Serial.print(schalter2);
  Serial.print(schalter3);
  Serial.print("  A0 ist ");
  Serial.print (joystick0);
  Serial.print(" A1 ist ");
  Serial.println(joystick1);



  transmit(n);


  delay(500);
}

unsigned long encode(long code) {
  long value = code | PREFIX;
  return value;
  // return code;
}

void transmit(long code) {

  long msg = encode(code);
  Serial.print("code:");
  Serial.print(code);
  Serial.print(", encoded:");
  Serial.println(msg);
  mySwitch.send(msg, MSG_LENGTH);
}