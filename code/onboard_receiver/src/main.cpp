#include <stdbool.h>
#include <RCSwitch.h>
// #include <debug.h>
#include <util.h>

#define RECEIVER_PIN 0     // entspircht pin 2
#define PREFIX 0xff000000  //prefix für empfang der nachricht
#define PREFIX_MASK 0x00ffffff


// Tauchzelle 1
#define TAUCHZELLEENA 5
#define TAUCHZELLE10 6
#define TAUCHZELLE11 7
#define TAUCHZELLENSTOP1 12


// Tauchzelle 2
#define TAUCHZELLEIN3 8
#define TAUCHZELLEIN4 9
#define TAUCHZELLEENB 10
#define TAUCHZELLENSTOP2 11

#define TAUCHZELLENGESCHWINDIGKEIT 150

#define CHECK_BIT(var,pos) ((var) & (1<<(pos))) != 0


bool tauchzellenstopp1 = false;
bool tauchzellenstopp2 = false;

bool tauchzelle1ausgefahren = false;
bool tauchzelle2ausgefahren = false;

unsigned long EINFAHRZEIT = 15000;

unsigned long EINFAHRTTS1 = 0;
unsigned long EINFAHRTTS2 = 0;

int pin = 1;
int pin1;

int baud = 9600;
int schalter1;
int schalter2;
int schalter3;
int joystick0;
int joystick1;

const int TPVO = 30;
const int TPVU = 32;
const int TPVL = 34;
const int TPVR = 36;

const int TPHO = 39;
const int TPHU = 41;
const int TPHL = 45;
const int TPHR = 44;

RCSwitch mySwitch = RCSwitch();

void setup() {
  pinMode(TAUCHZELLENSTOP1, INPUT_PULLUP);
  pinMode(TAUCHZELLENSTOP2, INPUT_PULLUP);

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

// Tauchzelle 2
void turnTZ2r(){
    digitalWrite(TAUCHZELLEIN3, LOW);
    digitalWrite(TAUCHZELLEIN4, HIGH);
}

void turnTZ2l(){
    digitalWrite(TAUCHZELLEIN3, HIGH);
    digitalWrite(TAUCHZELLEIN4, LOW);
}

void stopTZ2(){
   digitalWrite(TAUCHZELLEIN3, LOW);
   digitalWrite(TAUCHZELLEIN4, LOW);
}

void debugJParsedConfig(JCD jcd){
  String r = "rechts";
  String l = "links";
  String h = "hoch";
  String ru = "runter";
  String n = "neutral";
  Serial.print("J0 ");
  Serial.print("x: ");
  if (jcd.j0.x.none){
    Serial.print(n); 
    // data -> j1 -> right = (data & 0b01000000) >> 6;
  } 
  else {
      if (jcd.j0.x.right) Serial.print(r);
      else if (jcd.j0.x.left) Serial.print(l);
  }
  Serial.print(" y: ");
  if (jcd.j0.y.none) Serial.print(n);
  else {
    if (jcd.j0.y.up) Serial.print(h);
    else if(jcd.j0.y.down) Serial.print(ru);
  }
  
  Serial.println("");
  Serial.print("J1:");
  Serial.print(" x: ");
  if (jcd.j1.x.none) Serial.print(n);
  else {
      if (jcd.j1.x.right) Serial.print(r);
      else if (jcd.j1.x.left) Serial.print(l);

  }
  Serial.print(" y: ");
  if (jcd.j1.y.none) Serial.print(n);
  else {
    if (jcd.j1.y.up) Serial.print(h);
    else if (jcd.j1.y.down) Serial.print(ru);

  }
      Serial.println("");
    // Serial.print("data:");
    // Serial.print(data);
    // Serial.println("");
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
      //Steuerung Tauchzellen
      schalter1 = CHECK_BIT(code, 0);
      schalter2 = CHECK_BIT(code, 1);
      schalter3 = CHECK_BIT(code, 2);

      //Steuerung Seitentrieb
      long jcd_raw = bitrange(code, 8, 3);


      Serial.print("Receiving: ");
      Serial.print("S1:");
      Serial.print(schalter1);
      Serial.print(" | S2:");
      Serial.print(schalter2);
      // Serial.print(" | S3:");
      // Serial.print(schalter3);
      // Serial.print(" | Joystick raw:");
      // Serial.println(joystick_data);
      showJConfig(jcd_raw);
      // debug(mySwitch);

      JCD jcd = readJSData(jcd_raw);
      debugJParsedConfig(jcd);


      //TZ1
      if (digitalRead(TAUCHZELLENSTOP1) == 0){
        tauchzellenstopp1 = true;
        tauchzelle1ausgefahren = true;
        Serial.print(" Endsensor1:stop");
      }
      else {
        tauchzellenstopp1 = false;
      }

      if (schalter1) {
        if (tauchzellenstopp1) {
          stopTZ1();
          Serial.print(" TZ1:stop");
          EINFAHRTTS1 = millis();
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
          if (millis() - EINFAHRTTS1 > EINFAHRZEIT){
            //genug eiungefahren, stop
            Serial.print(" TZ1:eingefahren stop");
            stopTZ1();
            EINFAHRTTS1 = 0;
            tauchzelle1ausgefahren = false;  
          }
        }
      }
      Serial.println("");

      //TZ2
      if (digitalRead(TAUCHZELLENSTOP2) == 0){
        tauchzellenstopp2 = true;
        tauchzelle2ausgefahren = true;
        Serial.print(" Endsensor2:stop");
      }
      else {
        tauchzellenstopp2 = false;
      }
      
      if (schalter2) {
        if (tauchzellenstopp2) {
          stopTZ2();
          Serial.print(" TZ2:stop");
          EINFAHRTTS2 = millis();
        }
        //ausfahren
        else {
          Serial.print(" TZ2:ausfahren");
          tauchzelle2ausgefahren = false;
          turnTZ2l();
        }
      }
    //schalter aus
      else {
        if (tauchzelle2ausgefahren){
          //einfahren
          Serial.print(" TZ2:einfahren");
          turnTZ2r();
          if (millis() - EINFAHRTTS2 > EINFAHRZEIT){
            //genug eingefahren, stop
            Serial.print(" TZ2:eingefahren stop");
            stopTZ2();
            EINFAHRTTS2 = 0;
            tauchzelle2ausgefahren = false;  
          }
        }
      }
      Serial.println("#################"); 

    analogWrite(TAUCHZELLEENA, TAUCHZELLENGESCHWINDIGKEIT);
    analogWrite(TAUCHZELLEENB, TAUCHZELLENGESCHWINDIGKEIT);
    }
    else {
      Serial.print("Noise: ");
      Serial.println(code);
    }

    mySwitch.resetAvailable();
  }
}