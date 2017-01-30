#include <Arduino.h>

#include "bitword.h"

void BitWord::enq(bool b) {
  bitWrite(buf[(write_pos>>3)&7], write_pos&7, b);
  write_pos++;
}

void BitWord::enq(uint8_t b, uint8_t len) {
  for (int i = 0; i < len; i++) {
	  enq(bitRead(b, i) != 0);
  }
}

bool BitWord::deq() {
  bool r = val(0);
  read_pos++;
  return r;
}

bool BitWord::val(uint8_t i) {
  i += read_pos;
  return bitRead(buf[(i>>3)&7], i&7) != 0;
}

uint8_t BitWord::len() {
  return write_pos - read_pos;
}

uint64_t BitWord::word() {
  int n = len();
  uint64_t v = 0;
  for (int i = 0; i < n; i++) {
    bitWrite(v, i, val(i));
  }
  return v;
}

void BitWord::clear() {
  read_pos = 0;
  write_pos = 0;
}
