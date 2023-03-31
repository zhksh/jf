#include <stdbool.h>
#include <RCSwitch.h>
#include <debug.h>
#include <util.h>
#include <vals.h>


#define RECEIVER_PIN 0     // entspircht pin 2



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


RCSwitch receiver = RCSwitch();

Joystick j0 = Joystick("J0");
Joystick j1 = Joystick("J1");

TZ tz0 = {tz1_maxausfahrt, 
          tz1_maxeinfahrt, 
          TZ1_PIN0, 
          TZ1_PIN1, 
          TZ1_PIN_STOP, 
          TZ1_PIN_SPEED,
          "TZ1"
      };

TZ tz1 = {tz2_maxausfahrt,
          tz2_maxeinfahrt,
          TZ2_PIN0,
          TZ2_PIN1,
          TZ2_PIN_STOP,
          TZ2_PIN_SPEED,
          "TZ2"
     };
     

void setup() {
  pinMode(TZ1_PIN_STOP, INPUT_PULLUP);
  pinMode(TZ2_PIN_STOP, INPUT_PULLUP);

  Serial.begin(baud);
  Serial.println("Receiver setup");
  receiver.enableReceive(RECEIVER_PIN);

  tz0.setSpeed(TZ1_PIN_SPEED);
  tz1.setSpeed(TZ2_PIN_SPEED);
  // pinMode(TEST_PIN, INPUT);
}


void loop() { 
  if (receiver.available()) {  // Wenn ein Code Empfangen wird...
    unsigned long data = receiver.getReceivedValue();

    // bool legit = checkPrefix(code, decoded);
    if (islegit(data, CONTROL_RC_PREFIX, CONTROL_RC_PREFIX_LEN, CONTROL_RC_PREFIX_POS)) {
      // if (code == 1) {
      //   digitalWrite(pin1, HIGH);
      // }
      //Steuerung Tauchzellen
      schalter1 = CHECK_BIT(data, 0);
      schalter2 = CHECK_BIT(data, 1);
      // schalter3 = CHECK_BIT(code, 2);

      //Steuerung Seitentrieb
      long jcd_raw = BIT_RANGE(data, 8, 3);
      j0.parse(BIT_RANGE(jcd_raw, 4, 0));
      j1.parse(BIT_RANGE(jcd_raw, 4, 4));

      Serial.print("Receiving: ");
      Serial.print("S1:");
      Serial.print(schalter1);
      Serial.print(" | S2:");
      Serial.print(schalter2);
    }
    else {
      Serial.print("Noise: ");
      Serial.println(data);
    }
    receiver.resetAvailable();
  }

  // schalter1 = digitalRead(TEST_PIN);
  // schalter2 = schalter1; 
  // Serial.print("PIN SIGNAL: ");
  // Serial.print(schalter1);
  // Serial.println("");

  run |= schalter1;
  if (run){
    tz0.update(schalter1);
    tz1.update(schalter2);   
  }

}