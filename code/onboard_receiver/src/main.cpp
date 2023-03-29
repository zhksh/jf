#include <stdbool.h>
#include <RCSwitch.h>
#include <debug.h>
#include <util.h>

#define RECEIVER_PIN 0     // entspircht pin 2
#define PREFIX 0xff000000  //prefix für empfang der nachricht
#define PREFIX_MASK 0x00ffffff


// Tauchzelle 1
#define TZ1_PIN_SPEED 5
#define TZ1_PIN0 6
#define TZ1_PIN1 7
#define TZ1_PIN_STOP 12


// Tauchzelle 2
#define TZ2_PIN0 8
#define TZ2_PIN1 9
#define TZ2_PIN_SPEED 10
#define TZ2_PIN_STOP 11

#define TZ_SPEED 150

#define CHECK_BIT(var,pos) ((var) & (1<<(pos))) != 0


bool tz1_ausgefahren = false;
unsigned long tz1_maxeinfahrt = 15000;
unsigned long tz1_maxausfahrt = 4000;

bool tz2_ausgefahren = false;
unsigned long tz2_maxeinfahrt = 15000;
unsigned long tz2_maxausfahrt = 4000;


// int TEST_PIN = 2;
// int pin1;

boolean run = false;

int baud = 9600;
int schalter1 = 0;
int schalter2 = 0;

const int TPVO = 30;
const int TPVU = 32;
const int TPVL = 34;
const int TPVR = 36;

const int TPHO = 39;
const int TPHU = 41;
const int TPHL = 45;
const int TPHR = 44;






RCSwitch mySwitch = RCSwitch();

Joystick j0 = Joystick("J0");
Joystick j1 = Joystick("J1");

TZ tz0 = {tz1_maxausfahrt, 
          tz1_maxeinfahrt, 
          TZ1_PIN0, 
          TZ1_PIN1, 
          TZ1_PIN_STOP, 
          TZ1_PIN_SPEED,
          "TZ1"};

TZ tz1 = {tz2_maxausfahrt,
          tz2_maxeinfahrt,
          TZ2_PIN0,
          TZ2_PIN1,
          TZ2_PIN_STOP,
          TZ2_PIN_SPEED,
          "TZ2"
     };
     
tz1.setSpeed(TZ1_PIN_SPEED);
tz2.setSpeed(TZ2_PIN_SPEED);



void setup() {
  pinMode(TZ1_PIN_STOP, INPUT_PULLUP);
  pinMode(TZ2_PIN_STOP, INPUT_PULLUP);

  Serial.begin(baud);
  Serial.println("Receiver setup");
  mySwitch.enableReceive(RECEIVER_PIN);
  // pinMode(TEST_PIN, INPUT);
}


void loop() { 
  if (mySwitch.available()) {  // Wenn ein Code Empfangen wird...
    unsigned long code = mySwitch.getReceivedValue();
    long decoded = decode(code, PREFIX_MASK);  

    // bool legit = checkPrefix(code, decoded);
    if (islegit(code, decoded, PREFIX)) {
      // if (code == 1) {
      //   digitalWrite(pin1, HIGH);
      // }
      //Steuerung Tauchzellen
      schalter1 = CHECK_BIT(code, 0);
      schalter2 = CHECK_BIT(code, 1);
      // schalter3 = CHECK_BIT(code, 2);

      //Steuerung Seitentrieb
      long jcd_raw = bitrange(code, 8, 3);
      j0.parse(bitrange(jcd_raw, 4, 0));
      j1.parse(bitrange(jcd_raw, 4, 4));

      Serial.print("Receiving: ");
      Serial.print("S1:");
      Serial.print(schalter1);
      Serial.print(" | S2:");
      Serial.print(schalter2);
    }
    else {
      Serial.print("Noise: ");
      Serial.println(code);
    }
    mySwitch.resetAvailable();
  }

  // schalter1 = digitalRead(TEST_PIN);
  // schalter2 = schalter1; 
  // Serial.print("PIN SIGNAL: ");
  // Serial.print(schalter1);
  // Serial.println("");

  run |= schalter1;
  if (run){
    tz1.update(schalter1);
    tz2.update(schalter2);

   
  }

  
}