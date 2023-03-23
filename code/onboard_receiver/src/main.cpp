#include <stdbool.h>
#include <RCSwitch.h>
#include <debug.h>

#define RECEIVER_PIN 0     // entspircht pin 2
#define PREFIX 0xff000000  //prefix für empfang der nachricht
#define PREFIX_MASK 0x00ffffff

#define TAUCHZELLEENA 5
#define TAUCHZELLE10 6
#define TAUCHZELLE11 7

#define TAUCHZELLEIN3 8
#define TAUCHZELLEIN4 9
#define TAUCHZELLEENB 10
#define TAUCHZELLENGESCHWINDIGKEIT 150

#define TAUCHZELLENSTOP2 11
#define TAUCHZELLENSTOP1 12

#define TESTPIN 2


#define CHECK_BIT(var,pos) ((var) & (1<<(pos))) != 0


bool tauchzellenstopp1 = false;
bool tauchzellenstopp2 = false;

bool tauchzelle1ausgefahren = false;
bool tauchzelle2ausgefahren = false;

int EINFAHRZEIT = 5000;

int EINFAHRT1 = 0;

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
  pinMode(TAUCHZELLENSTOP1, INPUT_PULLUP);
  Serial.begin(baud);
  Serial.println("Receiver setup");
  mySwitch.enableReceive(RECEIVER_PIN);
}

long decode(long msg) {
  return msg & PREFIX_MASK;
}

bool checkPrefix(unsigned long original_msg, long  decoded) {
  bool passed = (decoded | PREFIX) == original_msg;
  // Serial.print("msg");
  // Serial.print(original_msg);
  return passed;
}

void debug(RCSwitch receiver){
  output(receiver.getReceivedValue(), receiver.getReceivedBitlength(), receiver.getReceivedDelay(), receiver.getReceivedRawdata(),receiver.getReceivedProtocol());
}

long bitrange(long msg, int len, int pos){
  long mask = (1 << len)-1;
  // Serial.println(mask);
  long result = msg >> pos;
  return result & mask;
}


void showJConfig(int data){
  String r = "rechts";
  String l = "links";
  String h = "hoch";
  String ru = "runter";
  String n = "neutral";

  Serial.print("J0 ");
  Serial.print("x: ");
  if ((data & 0b01000000) >> 6) Serial.print(n);
  else {
      if ((data & 0b10000000)) Serial.print(r);
      else Serial.print(l);

  }
  Serial.print(" y: ");
  if ((data & 0b00010000) >> 4) Serial.print(n);
  else {
    if ((data & 0b00100000)) Serial.print(h);
    else Serial.print(ru);
  }
  
  Serial.println("");
  Serial.print("J1:");
  Serial.print(" x: ");
  if ((data & 0b00000100) >> 2) Serial.print(n);
  else {
      if ((data & 0b00001000)) Serial.print(r);
      else Serial.print(l);

  }
  Serial.print(" y: ");
  if ((data & 0b00000001)) Serial.print(n);
  else {
    if ((data & 0b00000010)) Serial.print(h);
    else Serial.print(ru);

  }
      Serial.println("");
    // Serial.print("data:");
    // Serial.print(data);
    // Serial.println("");
}


void turnTZ1r(){
    digitalWrite(TAUCHZELLE10, LOW);
    digitalWrite(TAUCHZELLE11, HIGH);
}

void turnTZ1l(){
    digitalWrite(TAUCHZELLE10, HIGH);
    digitalWrite(TAUCHZELLE11, LOW);
}

void stopTZ1(){
   digitalWrite(TAUCHZELLE10, LOW);
   digitalWrite(TAUCHZELLE11, LOW);
}

void loop() {
  if (mySwitch.available()) {  // Wenn ein Code Empfangen wird...
    unsigned long code = mySwitch.getReceivedValue();
    long decoded = decode(code);  

    bool legit = checkPrefix(code, decoded);
    if (legit) {
      if (code == 1) {
        digitalWrite(pin1, HIGH);
      }

      schalter1 = CHECK_BIT(code, 0);
      schalter2 = CHECK_BIT(code, 1);
      schalter3 = CHECK_BIT(code, 2);

      long joystick_data = bitrange(code, 8, 3);
      // joystick1 = bitrange(code, 10, 13);


  Serial.print("Receiving: ");
  Serial.print("S1:");
  Serial.print(schalter1);
  //Auslesen der Tauchzellensensoren:
  if (digitalRead(TAUCHZELLENSTOP1) == 0){
    tauchzellenstopp1 = true;
    tauchzelle1ausgefahren = true;
    Serial.print(" Endsensor1:stop");

  }
  else {
    tauchzellenstopp1 = false;
  }

  /*statt dem schalter1, der normalerweise vom Sender empfangen wird, wird jetzt der Testpin
    verwendet. Ist der Testpin an, soll die Tauchzelle anfahren.*/
  // schalter1 = digitalRead(TESTPIN);

  if (schalter1) {
    if (tauchzellenstopp1) {
      stopTZ1();
      Serial.print(" TZ1:stop");

    }
    //ausfahren
    else {
      Serial.print(" TZ1:ausfahren");
      tauchzelle1ausgefahren = false;
      turnTZ1l();
    }
  }
  //schalter aus
  else {
    if (tauchzelle1ausgefahren){
      //einfahren
      Serial.print(" TZ1:einfahren");
      turnTZ1r();
      EINFAHRT1++;
      if (EINFAHRT1 >= EINFAHRZEIT){
        //genug eiungefahren, stop
        Serial.print(" TZ1:eingefahren stop");
        stopTZ1();
        EINFAHRT1 = 0;
        tauchzelle1ausgefahren = false;  
      }
    }
  }

  Serial.println("#######");





    /*Zweite Tauchzelle wird noch nicht getestet:

      digitalWrite(TAUCHZELLEIN3, schalter2);
      if (tauchzellenstopp2)
      digitalWrite(TAUCHZELLEIN4, schalter2);  //tauchzelle ausschalten
      else
      digitalWrite(TAUCHZELLEIN4, 1 - schalter1);

    */

    analogWrite(TAUCHZELLEENA, TAUCHZELLENGESCHWINDIGKEIT);
    // analogWrite(TAUCHZELLEENB, TAUCHZELLENGESCHWINDIGKEIT);


      // Serial.print("Receiving: ");
      // Serial.print("S1:");
      // Serial.print(schalter1);
      // Serial.print(" | S2:");
      // Serial.print(schalter2);
      // Serial.print(" | S3:");
      // Serial.print(schalter3);
      // Serial.print(" | Joystick raw:");
      // Serial.println(joystick_data);
      // showJConfig(joystick_data);
      // debug(mySwitch);

    }
   else {
      Serial.print("Noise: ");
      Serial.println(code);
    }

    mySwitch.resetAvailable();  // Hier wird der Empfänger "resettet"
  }
}