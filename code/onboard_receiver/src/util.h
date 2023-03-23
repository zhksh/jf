#include <RCSwitch.h>
#include <debug.h>


struct JData {
  bool left;
  bool right;
  bool none;
};

struct JoystickControlData {
  JData j1;
  JData j2;
};

long bitrange(long msg, int len, int pos){
  long mask = (1 << len)-1;
  // Serial.println(mask);
  long result = msg >> pos;
  return result & mask;
}

void debug(RCSwitch receiver){
  output(receiver.getReceivedValue(), receiver.getReceivedBitlength(), receiver.getReceivedDelay(), receiver.getReceivedRawdata(),receiver.getReceivedProtocol());
}