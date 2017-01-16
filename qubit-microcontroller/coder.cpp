#include "coder.h"

uint8_t crc8(uint8_t* bytes, int len, uint8_t gen = 0x31) {
  uint8_t crc = 0;
  for (int i = 0; i < len; i++) {
    crc ^= bytes[i];
    for (int j = 0; j < 8; j++) {
		  int b = crc & 0x80;
		  crc <<= 1;
      if (b) {
        crc ^= gen;
      }
    }
  }
  return crc;
} 

CodeReceiver::CodeReceiver(unsigned long pulse_duration, unsigned long message_length) {
  this->pulse_duration = pulse_duration;
  this->message_length = message_length + 1;
}

bool CodeReceiver::did_read_at(bool read_val, unsigned long read_time) {
  if (!has_read_been_called) {
    last_val = read_val;
    last_transition_time = read_time;
    has_read_been_called = true;
    return false;
  }
  
  if (did_receive || last_val == read_val || buf_len >= MAX_MSG_LEN*8) {
		return did_receive;
	}

  last_val = read_val;
	unsigned long dt = read_time - last_transition_time;
	last_transition_time = read_time;
	if (dt < pulse_duration/2.0 || dt > pulse_duration*5.0/2.0) {
		return false; // Bad value. Ignore.
	}

	bool val = dt > pulse_duration * 3.0 / 2.0;
	buf[buf_len>>3] |= (val ? 1 : 0) << (buf_len&7);
	buf_len++;
	
  int msg_bit_len = message_length * 8;
  if (buf_len >= msg_bit_len) {
		int offset = buf_len - msg_bit_len;
    for (int i = 0; i < message_length; i++) {
      received_buf[i] = 0;
    }
		for (int d = 0; d < msg_bit_len; d++) {
      int j = d + offset;
      bool x = ((buf[j>>3]>>(j&7)) & 1) != 0;
			received_buf[d>>3] |= (x ? 1 : 0) << (d & 7);
		}
		int e = message_length - 1;
    auto r = crc8(received_buf, e);
		did_receive = r == received_buf[e];
	}

	return did_receive;
}

void CodeSender::append(bool b) {
  sends[send_len>>3] |= (b?1:0)<<(send_len&7);  
  send_len++;
}

CodeSender::CodeSender(uint8_t* msg, unsigned long pulse_duration, unsigned long message_length, unsigned long start_time) {
  this->pulse_duration = pulse_duration;
  this->start_time = start_time;

  append(false);
  append(false);
  bool b = true;
  for (int i = 0; i <= message_length; i++) {
    uint8_t v = i < message_length ? msg[i] : crc8(msg, message_length);
    for (int j = 0; j < 8; j++) {
      int nk = (v >> j) & 1 != 0 ? 2 : 1;
      for (int k = 0; k < nk; k++) {
        append(b);
      }
      b ^= true;
    }
  }
  append(b);
  append(b);
}

bool CodeSender::value_to_write_at(unsigned long time) {
  unsigned long n;
  if (time < start_time) {
    n = 0;
  } else if (is_done_at(time)) {
    n = send_len - 1;
  } else {
    n = (time - start_time) / pulse_duration;
  }
  return ((sends[n>>3]>>(n&7)) & 1) != 0;
}

bool CodeSender::is_done_at(unsigned long time) {
  return time > start_time + send_len * pulse_duration;
}
