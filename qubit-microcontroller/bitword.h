//
// A ring buffer that can hold up to 64 bits.
//

#ifndef BIT_WORD_H
#define BIT_WORD_H

#include <stdint.h>

class BitWord {
private:
  volatile uint8_t buf[8] {0};
  volatile uint8_t read_pos = 0;
  volatile uint8_t write_pos = 0;

public:
  void enq(bool b);
  void enq(uint8_t b, uint8_t len = 8);
  bool deq();
  bool val(uint8_t i);
  uint8_t len();
  uint64_t word();
  void clear();
};

#endif
