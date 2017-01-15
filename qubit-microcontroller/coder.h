#ifndef CODER_H
#define CODER_H

uint8_t crc8();

class CodeReceiver {
private:
  bool buf[1 << (4 + 8)];
  bool last_val = false;
  int buf_len = 0;
  bool has_read_been_called = false;
  unsigned long last_transition_time = 0;
  unsigned long pulse_duration;
  unsigned long message_length;

public:
  bool did_receive = false;
  uint8_t received_buf[1 << 4];
  CodeReceiver(unsigned long pulse_duration, unsigned long message_length);
  bool did_read_at(bool read_val, unsigned long read_time);
};

class CodeSender {
private:
  unsigned long send_len = 0;
  unsigned long start_time;
  unsigned long pulse_duration;
  bool sends[1 << 12];

public:
  CodeSender(uint8_t* msg, unsigned long pulse_duration, unsigned long message_length, unsigned long start_time);
  bool value_to_write_at(unsigned long time);
};

#endif
