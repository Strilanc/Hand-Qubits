#include <Arduino.h>
#include "contact.h"
#include "coder.h"
#include "bitword.h"
#include "hardware_timer_1.h"

#define CONTACT_PIN 2

#define LISTENING 0
#define DID_RECEIVE_NOW_WAITING 1
#define SENDING 2

#define MAGIC ((uint8_t)0x42)
#define EXP_MSG_LEN 3
#define PULSE_DURATION 500LL
#define SEND_DURATION (PULSE_DURATION*8*(EXP_MSG_LEN+4))
#define LISTEN_DURATION (SEND_DURATION + 5000)
#define CYCLE_DURATION (SEND_DURATION+LISTEN_DURATION)

static volatile int state = SENDING;
static volatile unsigned long next_transition_time;
static BitWord received_bits;
static BitWord send_bits;
static uint8_t byte_to_send;
volatile uint8_t last_received_message;
bool is_in_contact;

void contact_setup() {
  // Currently no setup steps. Just here for consistency with the setup+loop method pattern.
}

void contact_set_byte_to_send(uint8_t message_byte) {
  byte_to_send = message_byte;
}

int contact_get_current_other_message() {
  return is_in_contact ? last_received_message : -1;
}

void transition_out() {
  // Turn off interrupts and clear data.
  detachInterrupt(digitalPinToInterrupt(CONTACT_PIN));
  stop_periodic_interrupt_timer_1();
  received_bits.clear();
  send_bits.clear();
}

void transition_to_listening(unsigned long fromTime) {
  noInterrupts();
  transition_out();
  next_transition_time = fromTime + LISTEN_DURATION + random(0, 2) * CYCLE_DURATION;
  pinMode(CONTACT_PIN, INPUT);
  state = LISTENING;
  interrupts();

  // Watch for voltage flips.
  attachInterrupt(digitalPinToInterrupt(CONTACT_PIN), [](){
    if (state != LISTENING) {
      return;
    }

    // Data is encoded in the duration between changes, so measure that.
    static unsigned long last_change_time = 0;
    unsigned long t = micros();
    unsigned long dt = t - last_change_time;
    last_change_time = t;

    // If way out of range, ignore.
    if (dt < PULSE_DURATION * 0.5 || dt > PULSE_DURATION * 2.5) {
      return;
    }

    // A single pulse duration is a 0. A double duration is a 1. Split around 1.5.
    received_bits.enq(dt > PULSE_DURATION * 1.5);

    // Have we got enough bits yet?
    if (received_bits.len() != EXP_MSG_LEN*8) {
      return;
    }

    // Grab potential message.
    auto v = received_bits.word();
    received_bits.deq(); // Next time we won't need the oldest bit.

    // Does it have the right magic byte and a correct CRC?
    byte* vb = (byte*) (void*) &v;
    int e = EXP_MSG_LEN-1;
    if (vb[0] != MAGIC || vb[e] != crc8(vb, e)) {
      return;
    }

    // Message received.
    last_received_message = (received_bits.word() >> 8) & 0xFF;
    transition_out();
    state = DID_RECEIVE_NOW_WAITING;

    // The sender is about to start listening, so now we know when to start sending.
    // (This line causes a lock-on effect, where the send/receive cycle rate jumps when the two agents can hear each other.)
    next_transition_time = micros() + PULSE_DURATION*5;
  }, CHANGE);
}

void prepare_send_bits() {
  // Padding to make it easier for the other side to latch on.
  send_bits.enq(true);
  send_bits.enq(true);

  // Message w/ leading magic byte and trailing crc.
  send_bits.enq(MAGIC);
  send_bits.enq(byte_to_send);
  uint8_t dat[] {MAGIC, byte_to_send};
  send_bits.enq(crc8(dat, sizeof(dat)));

  // Padding to make it easier for the other side to notice the end of the message.
  send_bits.enq(true);
  send_bits.enq(true);
}

void transition_to_sending(unsigned long from_time) {
  noInterrupts();
  transition_out();
  pinMode(CONTACT_PIN, OUTPUT);
  state = SENDING;
  next_transition_time = from_time + SEND_DURATION;
  prepare_send_bits();
  interrupts();
  
  start_periodic_interrupt_timer_1(PULSE_DURATION, [](){
    if (state != SENDING) {
      return;
    }
    static bool hold = false;
    static bool cur_pin_val = false;

    // Wait one more pulse duration before toggling?
    if (hold) {
      hold = false;
      return;
    }

    // When done sending, immediately switch to listening.
    if (send_bits.len() == 0) {
      transition_to_listening(next_transition_time);
      return;
    }

    // Data is encoded into durations between changes, so remember whether to hold for an extra duration before toggling.
    hold = send_bits.deq();
    cur_pin_val ^= true;
    digitalWrite(CONTACT_PIN, cur_pin_val);
  });  
}

void contact_loop() {
  auto t = micros();
  
  noInterrupts();
  if (next_transition_time <= t) {
    switch (state) {
    case SENDING:
      transition_to_listening(t);
      break;
    default: // LISTENING or DID_RECEIVE_NOW_WAITING
      // If we exited the LISTENING state, a message was received and we are in contact.
      is_in_contact = state == DID_RECEIVE_NOW_WAITING;
      transition_to_sending(t);
    }
  }
  interrupts();
}

