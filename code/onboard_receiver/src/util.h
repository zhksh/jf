#include <RCSwitch.h>
#include <debug.h>

struct XAxis {
  bool right;
  bool left;
  bool none;
};

struct YAxis {
  bool up;
  bool down;
  bool none;
};

struct JData {
  XAxis x;
  YAxis y;
};

struct JCD {
  JData j0;
  JData j1;
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

JCD readJSData(int data){
  JCD cd = {};
  cd.j1.x.none = (data & 0b01000000) >> 6;
  cd.j0.x.right = (data & 0b10000000) >> 7;
  cd.j0.x.left = 1-((data & 0b10000000) >> 7);

  cd.j0.y.none = (data & 0b00010000) >> 4;
  cd.j0.y.up = (data & 0b00100000) >> 5;
  cd.j0.y.down = 1-((data & 0b00100000) >> 5);


  cd.j1.x.none = (data & 0b00000100) >> 2;
  cd.j1.x.right = (data & 0b00000010) >> 3;
  cd.j1.x.left = 1-((data & 0b00000010) >> 3);

  cd.j1.y.none = (data & 0b00000001);
  cd.j1.y.up = (data & 0b00000010) >> 1;
  cd.j1.y.down = 1-((data & 0b00000010) >> 1);


  return cd;
};