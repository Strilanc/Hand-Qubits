#include <Arduino.h>
#include "contact.h"
#include "coder.h"
#include "bitword.h"
#include "hardware_timer_1.h"

#define CONTACT_PIN A0

#define LISTENING 0
#define DID_RECEIVE_NOW_WAITING 1
#define SENDING 2

#define MAGIC ((uint8_t)0xA3)
#define EXP_MSG_LEN 3
#define EXP_MSG_BIT_LEN (EXP_MSG_LEN*8)
#define TICK_MICROS 512
#define SEND_TICKS ((EXP_MSG_BIT_LEN+1)*4)
#define LISTEN_TICKS (SEND_TICKS+8)

int state = SENDING;
volatile uint8_t byte_to_send = 0xFF;
volatile uint8_t last_received_message = 0xFF;
uint16_t ticks_until_next_transition = 0;

void do_hardware_loop(void);
void contact_setup() {
  start_periodic_interrupt_timer_1(TICK_MICROS, &do_hardware_loop);
}

void contact_loop() {
  // Work is done in hardware timer interrupts. Nothing to do here.
}

void contact_set_byte_to_send(uint8_t message_byte) {
  byte_to_send = message_byte;
}

uint8_t contact_get_byte_to_send() {
  return byte_to_send;
}

uint8_t contact_get_current_other_message() {
  return last_received_message;
}

void start_listening(uint16_t extra_ticks);
void start_sending(void);


uint8_t send_queue_bits = 0;
uint8_t send_queue_len = 0;
BitWord send_bits;

void start_sending() {
  pinMode(CONTACT_PIN, OUTPUT);
  state = SENDING;
  send_queue_len = 0;
  ticks_until_next_transition = SEND_TICKS;
  //ticks_until_next_transition = 0;

  // Give a bit of time to take control of voltage.
  send_bits.enq(true);

  // Message w/ leading magic byte and trailing crc.
  send_bits.enq(MAGIC);
  send_bits.enq(byte_to_send);
  uint8_t dat[] {MAGIC, byte_to_send};
  send_bits.enq(crc8(dat, sizeof(dat)));
}

void do_sending() {
  if (send_queue_len == 0) {
    // When done sending, immediately switch to listening.
    if (send_bits.len() == 0) {
      start_listening(ticks_until_next_transition);
      return;
    }
  
    // Queue next Manchester-encoded bit.
    send_queue_len = 4;
    bool val = send_bits.deq();
    send_queue_bits = val ? 0b0011 : 0b1100;
  }
  
  digitalWrite(CONTACT_PIN, (send_queue_bits & 1) != 0);
  send_queue_bits >>= 1;
  send_queue_len--;
}


uint8_t recent_readings[4];
uint8_t recent_readings_next_write = 0;
uint8_t recent_readings_len = 0;

uint64_t readings_manchester_mask[2];
uint8_t readings_len[2] {0};
int which_readings = 0;

void start_listening(uint16_t extra_ticks) {
  pinMode(CONTACT_PIN, INPUT);
  state = LISTENING;
  ticks_until_next_transition = extra_ticks + LISTEN_TICKS * random(1, 6);
  which_readings = 0;
  readings_len[0] = 0;
  readings_len[1] = 0;
  recent_readings_len = 0;
  recent_readings_next_write = 0;
}

void do_listening() {
  uint8_t prev = recent_readings[recent_readings_next_write];
  uint8_t cur = recent_readings[(recent_readings_next_write + 2) & 3];
  uint8_t next = (uint8_t) (analogRead(A0) >> 2);
  recent_readings[recent_readings_next_write] = next;
  recent_readings_next_write++;
  recent_readings_next_write &= 3;

  if (recent_readings_len < 4) {
    recent_readings_len++;
    return;
  }
  
  int high = max(prev, next);
  int low = min(prev, next);
  bool is_cur_high = cur >= high || (cur > low && abs(cur - high) < abs(cur - low));
  which_readings = 1 - which_readings;
  readings_manchester_mask[which_readings] <<= 1;
  readings_manchester_mask[which_readings] |= is_cur_high ? 1 : 0;
  if (readings_len[which_readings] < EXP_MSG_BIT_LEN*2) {
    readings_len[which_readings] += 1;
    return;
  }

  uint64_t read_bit_mask = readings_manchester_mask[which_readings];
  uint32_t received_bit_mask = 0;
  for (int i = 0; i < EXP_MSG_BIT_LEN; i++) {
     bool a = bitRead64(read_bit_mask, i*2);
     bool b = bitRead64(read_bit_mask, i*2+1);
  }
  for (int i = EXP_MSG_BIT_LEN - 1; i >= 0; i--) {
    bool a = bitRead64(read_bit_mask, i*2);
    bool b = bitRead64(read_bit_mask, i*2+1);
    if (a == b) {
      // Not manchester encoded.
      return; 
    }
    bitWrite(received_bit_mask, EXP_MSG_BIT_LEN-1-i, b);
  }

  // Does it have the right magic byte and a correct CRC?
  byte* vb = (byte*) (void*) &received_bit_mask;
  int e = EXP_MSG_LEN - 1;
  if (vb[0] != MAGIC || vb[e] != crc8(vb, e)) {
    return;
  }

  // Message received.
  state = DID_RECEIVE_NOW_WAITING;
  last_received_message = vb[1];

  // The sender is about to start listening, so now we know when to start sending.
  // (This line causes a lock-on effect, where the send/receive cycle rate jumps when the two agents can hear each other.)
  ticks_until_next_transition = 2;
}

void do_hardware_loop() {
  // Time to transition?
  if (ticks_until_next_transition-- == 0) {
    switch (state) {
    case LISTENING:
      //Serial.println("NO");
      last_received_message = 0xFF; // Failed to receive.
      start_sending();
      break;
    case DID_RECEIVE_NOW_WAITING:
      start_sending();
      break;
    case SENDING:
      start_listening(0);
      break;
    }
  }

  // Work work.
  switch (state) {
  case LISTENING:
    do_listening();
    break;
  case SENDING:
    do_sending();
    break;
  }
}

