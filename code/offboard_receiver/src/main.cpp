#include <Arduino.h>

#include <stdbool.h>
#include <RCSwitch.h>
#include <debug.h>

#define RECEIVER_PIN 1     // entspircht pin 2 , interrupt 0
#define PREFIX 0xd0000000  //prefix für empfang der nachricht
#define PREFIX_MASK 0x0fffffff

// #define TB_MASK ((1 << 10) - 1) << 17
#define TB_MASK    0b00000011111111100000000000000000
#define TEMP0_MASK 0b00000000000000011111111000000000




int baud = 9600;


RCSwitch mySwitch = RCSwitch();

void setup() {
  Serial.begin(baud);
  Serial.println("Receiver setup");
  mySwitch.enableReceive(RECEIVER_PIN);
}


long decode(long msg) {
  //this conversion cuts off the highest two bytes
  return msg & PREFIX_MASK;
}

bool checkPrefix(unsigned long original_msg, int decoded) {
  bool passed = (decoded | PREFIX) == original_msg;
  // Serial.print("msg");
  // Serial.print(original_msg);
  return passed;
}

void debug(RCSwitch s){
  output(mySwitch.getReceivedValue(), mySwitch.getReceivedBitlength(), mySwitch.getReceivedDelay(), mySwitch.getReceivedRawdata(),mySwitch.getReceivedProtocol());
}


void loop() {
  if (mySwitch.available()) {  // Wenn ein Code Empfangen wird...
    debug(mySwitch);

    unsigned long msg = mySwitch.getReceivedValue();
    long code = decode(msg);    
    Serial.println((msg & TB_MASK) >> 16);
    Serial.println((msg & TEMP0_MASK) >> 8);


    bool legit = checkPrefix(msg, code);
    if (legit) {
      Serial.print("Legit Code: ");
      Serial.println(code);

      // schalter1 = code % 2; //most right bit
      // schalter2 = (code - schalter1) / 2 % 2; //second most right bit
      // schalter3 = (code - 2 * schalter2 - schalter1) / 4 % 2; //third most right bit

      // joystick0 = (code - 4 * schalter3 - 2 * schalter2 - schalter1) / 8 % 1024; 
      // joystick1 = (code - 8 * joystick0 - 4 * schalter3 - 2 * schalter2 - schalter1) / 8192;

    }
   else {
    Serial.print("Noise: ");
    Serial.println(code);
  }


  mySwitch.resetAvailable();  // Hier wird der Empfänger "resettet"
}
}