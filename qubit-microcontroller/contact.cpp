#include <Arduino.h>
#include "contact.h"
#include "coder.h"
#include "sound.h"

#define LISTENING 0
#define DID_RECEIVE_NOW_WAITING 1
#define SENDING 2

#define PULSE_DURATION 5
#define SEND_DURATION 200
#define LISTEN_DURATION 300
#define CYCLE_DURATION (SEND_DURATION+LISTEN_DURATION)

static int state = SENDING;
static unsigned long next_time = 0;
static unsigned long start_time = 0;
static uint8_t msg[]{ 0x42, 0xF4 };
static CodeSender sender(msg, PULSE_DURATION, sizeof(msg), 0);
static CodeReceiver receiver(PULSE_DURATION, sizeof(msg));

void contact_setup() {
  pinMode(A0, INPUT);
}

void transition_to_listening(unsigned long fromTime) {
  state = LISTENING;
  pinMode(A0, INPUT);
  next_time = fromTime + LISTEN_DURATION + random(0, 2) * CYCLE_DURATION;
  receiver = CodeReceiver(PULSE_DURATION, sizeof(msg));
}

void contact_loop() {
  auto t = millis();
  if (next_time <= t) {
    switch (state) {
    case SENDING:
      transition_to_listening(t);
      break;
    default:
      if (state == LISTENING) {
        set_ticking(false);
      }
      state = SENDING;
      pinMode(A0, OUTPUT);
      start_time = t;
      next_time = t + SEND_DURATION;
      sender = CodeSender(msg, PULSE_DURATION, sizeof(msg), millis());
    }
  }

  int r;
  bool b;
  switch (state) {
  case LISTENING:
    r = analogRead(A0);
    if (r < 200 || r > 824) {
      Serial.println(r);
      receiver.did_read_at(r < 500, t);
    }

    if (receiver.did_receive) {
      state = DID_RECEIVE_NOW_WAITING;
      set_ticking(true);
      if (receiver.received_buf[0] == 0x42) {
        next_time = t + 50;
        // handle message
      }
    }
    break;

  case DID_RECEIVE_NOW_WAITING:
    Serial.println(receiver.received_buf[1]);
    break;

  case SENDING:
    digitalWrite(A0, sender.value_to_write_at(t) ? HIGH : LOW);
    Serial.println(sender.value_to_write_at(t) ? 550 : 450);
    if (sender.is_done_at(t)) {
      transition_to_listening(next_time);
    }
    break;
  }
}
