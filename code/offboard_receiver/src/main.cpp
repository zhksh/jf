#include <Arduino.h>
#include <tempHistory.h>

#include <stdbool.h>
#include <RCSwitch.h>
#include <debug.h>



#define RECEIVER_PIN 1     // entspircht pin 2 , interrupt 0

#define PREFIX 0xd0000000  //prefix für empfang der nachricht
#define PREFIX_MASK 0x0fffffff

#define TB_MASK    0x0fff0000
#define TB_POS     16



#define TEMP0_MASK 0x000000ff
#define TEMP0_POS  0


#define TEMP1_MASK 0x0000ff00
#define TEMP1_POS  8

#define BAUD  9600


RCSwitch mySwitch = RCSwitch();
TempHIst hist = TempHIst();

void setup() {
  Serial.begin(BAUD);
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

void debug(RCSwitch s){
  output(mySwitch.getReceivedValue(), mySwitch.getReceivedBitlength(), mySwitch.getReceivedDelay(), mySwitch.getReceivedRawdata(),mySwitch.getReceivedProtocol());
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

float convertTransToTemp(long transmitted){
  float temp = (float) transmitted/10;
  //we assume if tempt drops by more than 10deg cmp to avg its an overflow
  if ((temp + 10) < hist.getAvg()){
    temp += 25.6;
  } 
  hist.push(temp);

  return temp;
}

void loop() {
  if (mySwitch.available()) {  // Wenn ein Code Empfangen wird...
    // debug(mySwitch);
    unsigned long msg = mySwitch.getReceivedValue();
    long decoded = decode(msg);  

    bool legit = checkPrefix(msg, decoded);
    if (legit) {
      long tb_val =  (msg & TB_MASK) >> TB_POS;
      long temp0_val = (msg & TEMP0_MASK) >> TEMP0_POS;
      long temp1_val = (msg & TEMP1_MASK) >> TEMP1_POS;
      float temp0 = convertTransToTemp(temp0_val);
      float temp1 = convertTransToTemp(temp1_val);
      Serial.println("##############");
      Serial.println(temp0);
      Serial.println(temp1);



      float temp1_dec = (float) temp1_val/10;
      // printToSerial(tb_val, temp0_dec , temp1_dec, 0.0);
      // Serial.println(tb_val);
      // Serial.println(temp0_dec);
      // Serial.println(temp0_val);
      //   Serial.println(temp1_dec);
      // Serial.println(temp1_val);
    }
    else {
      Serial.print("Noise: ");
      debug(mySwitch);
    }


    mySwitch.resetAvailable();
  }
}