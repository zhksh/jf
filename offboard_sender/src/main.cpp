#include <RCSwitch.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <util.h>
#include <vals.h>
#include <debug.h>



// PINS
#define TRANSMITTER_PIN 10
#define SWITCH1  2
#define SWITCH2  3
#define SWITCH3  4
#define JOYSTICK0X  A1
#define JOYSTICK0Y  A0
#define JOYSTICK1X  A2
#define JOYSTICK1Y  A3


RCSwitch mySwitch = RCSwitch();

void transmit(long data) {

  long msg = addprefix(data, CONTROL_RC_PREFIX, CONTROL_RC_PREFIX_POS);
  // Serial.print("code:");
  // Serial.print(code);
  // Serial.print(", encoded:");
  // Serial.println(msg);
  mySwitch.send(msg, MSG_LENGTH);
}

int joystick2data(long j0x, long j0y, long j1x, long j1y){
  int data = 0;
  
  //j0x 7 j0x null 6   j0y 5 j0y null 4   j1x 3 j1x null 2    j1y 1 j1y null 0
  
  //j0 null pos
  if (j0x > 470 && j0x < 600 ) data |= 1 << 6;  
  //j0 rechts set bit
  else if (j0x < 470) data |= 1 << 7;

  if (j0y > 480 && j0y < 800) data |= 1 << 4;
  //j0 hoch set bit
  else if ( j0y < 480) data |= 1 << 5;

  if (j1x > 520 && j1x < 600) data |= 1 << 2;
  //j1 rechts set bit
  else if (j1x < 520) data |= 1 << 3;

  if (j1y > 460 && j1y < 650) data |= 1 << 0;
  //j1 hoch set bit
  else if (j1y < 460 ) data |= 1 << 1;

  return data;
}


void setup() {
  Serial.begin(BAUD);
  Serial.println("Sender setup");
  mySwitch.enableTransmit(TRANSMITTER_PIN);

  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(JOYSTICK0X, INPUT);
  pinMode(JOYSTICK0Y, INPUT);
  pinMode(JOYSTICK1X, INPUT);
  pinMode(JOYSTICK1Y, INPUT);
}


void loop() {
  long schalter1 = digitalRead(SWITCH1);
  long schalter2 = digitalRead(SWITCH2);
  long schalter3 = digitalRead(SWITCH3);
  long joystick0Y = analogRead(JOYSTICK0Y);
  long joystick0X = analogRead(JOYSTICK0X);
  long joystick1Y = analogRead(JOYSTICK1Y);
  long joystick1X = analogRead(JOYSTICK1X);

  //one byte long
  long jdata_encoded = joystick2data(joystick0X, joystick0Y, joystick1X, joystick1Y);

  long data;
  data |=  schalter1 << CONTROL_S1_POS; 
  data |=  schalter2 << CONTROL_S2_POS; 
  data |=  schalter3 << CONTROL_S3_POS; 
  // data |=  joystick0X << 3; 
  // data |=  joystick0Y << 13; 

  data |= jdata_encoded << CONTROL_J_POS;


  Serial.print("Sending: S1:");
  Serial.print(schalter1);
  Serial.print(" | S2:");
  Serial.print(schalter2);
  Serial.print(" | S3:");
  Serial.print(schalter3);
  Serial.print(" | J0X:");
  Serial.print(joystick0X);
  Serial.print(" | J0Y:");
  Serial.print(joystick0Y);
  Serial.print(" | J1X:");
  Serial.print(joystick1X);
  Serial.print(" | J1Y:");
  Serial.println(joystick1Y);
  showJConfig(jdata_encoded);

  transmit(data);

  delay(0);
}



