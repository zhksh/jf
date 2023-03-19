#include <Arduino.h>

#include <RCSwitch.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PREFIX 0xfe000000
#define MSG_LENGTH 32


// PINS
#define TRANSMITTER_PIN 10

#define TEMPSENSOR0_PIN  4
#define TEMPSENSOR1_PIN  5

#define SWITCH3  
#define JOYSTICK0  A0
#define JOYSTICK1  A1

#define TESENSOR A0


RCSwitch mySwitch = RCSwitch();

OneWire temp0Driver(TEMPSENSOR0_PIN);
DallasTemperature temp0Sensor(&temp0Driver);

OneWire temp1Driver(TEMPSENSOR1_PIN);
DallasTemperature temp1Sensor(&temp1Driver);

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
  temp0Sensor.begin();
  temp1Sensor.begin();

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
  Serial.print("DATA|");
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

float requestTemp(DallasTemperature sensor){
    float val;
    sensor.requestTemperatures(); // Send the command to get temperatures
    Serial.println("DONE");
    // After we got the temperatures, we can print them here.
    // We use the function ByIndex, and as an example get the temperature from the first sensor only.
    val = sensor.getTempCByIndex(0);
    // Check if reading was successful
    // if(val != DEVICE_DISCONNECTED_C) 
    // {
    //   Serial.print("Temperature for the device 1 (index 0) is: ");
    //   Serial.println(val);
    // } 
    // else
    // {
    //   Serial.println("Error: Could not read temperature data");
    // }
    return val;
}

void loop() {

  // float sensorValue = digitalRead(TESENSOR);
  float tbval = analogRead(TESENSOR);
  float val0 = requestTemp(temp0Sensor);
  float val1 = requestTemp(temp1Sensor);



  // sensorValue/=350;
  // sensorValue*=100;
  // sensorValue=100-sensorValue;
  printToSerial(tbval, val0, val1, 0.0);


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

