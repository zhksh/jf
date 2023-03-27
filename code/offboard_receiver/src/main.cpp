#include <Arduino.h>
#include <tempHistory.h>

#include <stdbool.h>
#include <RCSwitch.h>
#include <debug.h>


#define RECEIVER_PIN 0     //interrupt 0 == pin 2

#define PREFIX 0xd0000000  //codeword prefix 
#define PREFIX_MASK 0x0fffffff

#define TB_MASK    0x0fff0000
#define TB_POS     16

#define TEMP0_MASK 0x000000ff

#define TEMP1_MASK 0x0000ff00
#define TEMP1_POS  8

#define BAUD  9600


RCSwitch receiver = RCSwitch();
TempHistory hist = TempHistory();

void setup() {
  Serial.begin(BAUD);
  Serial.println("Receiver setup");
  receiver.enableReceive(RECEIVER_PIN);
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

void debug(RCSwitch reciever){
  output(receiver.getReceivedValue(), receiver.getReceivedBitlength(), receiver.getReceivedDelay(), receiver.getReceivedRawdata(),receiver.getReceivedProtocol());
}

void encodeForUI(float tb, float temp0, float temp1, float aux){
  Serial.print("DATA|");
  
  //key for ui
  Serial.print("tb");
  Serial.print(":");
  Serial.print(tb);
  Serial.print(";");

  //key for ui
  Serial.print("tmp0");
  Serial.print(":");
  Serial.print(temp0);
  Serial.print(";");

  //key for ui
  Serial.print("tmp1");
  Serial.print(":");
  Serial.print(temp1);
  Serial.print(";");

   //key for ui
  Serial.print("aux");
  Serial.print(":");
  Serial.print(aux);

  Serial.println("");
}


float convertTransToTemp(long transmitted){
  //we want to make a byte a float with one decimal and therefore need transmitted < 1000
  if (transmitted >= 1000) return (float) transmitted;
  float temp = (float) transmitted/10;
  //we assume if tempt drops by more than 20C cmp to avg and last value its an overflow
  //and add the cutoff value, which should be precisely 256/10
  if ((temp + 20) < hist.getAvg() &  hist.getLast() - temp  > 20){
    temp += 25.6;
  } 
  hist.push(temp);

  return temp;
}


void loop() {
  if (receiver.available()) {
    // debug(mySwitch);
    unsigned long msg = receiver.getReceivedValue();
    long decoded = decode(msg);  

    bool legit = checkPrefix(msg, decoded);
    bool legit = true;

    if (legit) {
      long tb_val =  (msg & TB_MASK) >> TB_POS;
      long temp1_val = (msg & TEMP1_MASK) >> TEMP1_POS;
      long temp0_val = msg & TEMP0_MASK;

      float temp0 = convertTransToTemp(temp0_val);
      float temp1 = convertTransToTemp(temp1_val);
      // Serial.println("##############");
      // Serial.println(temp0);
      // Serial.println(temp1);
      encodeForUI(tb_val, temp0, temp1, 0.0);
    }
    else {
      Serial.print("Noise: ");
      debug(receiver);
    }
    
    receiver.resetAvailable();
  }
}