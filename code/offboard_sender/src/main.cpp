#include <stdbool.h>
#include <RCSwitch.h>

#define RECEIVER_PIN 0     // entspircht pin 2
#define PREFIX 0xfe000000  //prefix für empfang der nachricht
#define PREFIX_MASK 0x00ffffff

#define TAUCHZELLEENA 5
#define TAUCHZELLEIN1 6
#define TAUCHZELLEIN2 7
#define TAUCHZELLEIN3 8
#define TAUCHZELLEIN4 9
#define TAUCHZELLEENB 10
#define TAUCHZELLENGESCHWINDIGKEIT 150

#define TAUCHZELLENSTOPP1 12
#define TAUCHZELLENSTOPP2 13

bool tauchzellenstopp1;
bool tauchzellenstopp2;

int pin = 1;
int pin1;

int baud = 9600;
int schalter1;
int schalter2;
int schalter3;
int joystick0;
int joystick1;

RCSwitch mySwitch = RCSwitch();

void setup() {
  Serial.begin(baud);
  Serial.println("Receiver setup");
  mySwitch.enableReceive(RECEIVER_PIN);
  tauchzellenstopp1 = false;
  tauchzellenstopp2 = false;
}


long decode(long msg) {
  //this conversion cuts off the highest two bytes
  return msg & PREFIX_MASK;
}

// void prinbin(RCSwitch s){
//   const char* b = dec2binWzerofill(s.getReceivedValue(), s.getReceivedBitlength());

//    Serial.print("Bit) Binary: ");
//    Serial.print( b );
// }
bool checkPrefix(unsigned long original_msg, int decoded) {
  bool passed = (decoded | PREFIX) == original_msg;
  // Serial.print("msg");
  // Serial.print(original_msg);

  return passed;
}




void loop() {
  if (mySwitch.available()) {  // Wenn ein Code Empfangen wird...


    unsigned long msg = mySwitch.getReceivedValue();
    long code = decode(msg);
    // Serial.print("msg width:");
    // Serial.println(sizeof(code));

    bool legit = checkPrefix(msg, code);
    if (legit) {
      // Serial.print("Code: ");
      // Serial.println(code);
      if (code == 1) {
        digitalWrite(pin1, HIGH);
      }

      schalter1 = code & 1;
      schalter2 = (code <<1)&1;
      schalter3 = (code <<2)%1;

      joystick0 = (code <<3)%1023;
      joystick1 = (code <<11)%1023;

      if (digitalRead(TAUCHZELLENSTOPP1) == 1)
        tauchzellenstopp1 = true;
      if (digitalRead(TAUCHZELLENSTOPP2) == 1)
        tauchzellenstopp2 = true;

      digitalWrite(TAUCHZELLEIN1, schalter1);
      

      if (tauchzellenstopp1)
        digitalWrite(TAUCHZELLEIN2, schalter1);  // tauchzelle ausschalte
      else
        digitalWrite(TAUCHZELLEIN2, 1 - schalter1);//tauczelle an

      digitalWrite(TAUCHZELLEIN3, schalter2);
      if (tauchzellenstopp2)
        digitalWrite(TAUCHZELLEIN4, schalter2);  //tauchzelle ausschalten
      else
        digitalWrite(TAUCHZELLEIN4, 1 - schalter1);

      analogWrite(TAUCHZELLEENA, TAUCHZELLENGESCHWINDIGKEIT);
      analogWrite(TAUCHZELLEENB, TAUCHZELLENGESCHWINDIGKEIT);

      Serial.print(schalter1);
      Serial.print(schalter2);
      Serial.println(schalter3);
      Serial.print("A0 ist");
      Serial.println(joystick0);
      Serial.print("A1 ist");
      Serial.print(joystick1);
    }
   else {
    Serial.print("Noise: ");
    Serial.println(code);
  }


  mySwitch.resetAvailable();  // Hier wird der Empfänger "resettet"
}
}