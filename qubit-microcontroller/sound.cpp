#include <Arduino.h>
#include "sound.h"

static int soundPin = A3;

#define QUEUE_CAP 128
typedef struct {
  unsigned long time;
  bool val;
} Item;
static Item queue[QUEUE_CAP];
static int queue_start = 0;
static int queue_len = 0;
static bool is_ticking = false;
static unsigned long next_time;

static void enqueue(Item item) {
  if (queue_len >= QUEUE_CAP) {
    return; // Silent drop when overloaded.
  }
  queue[(queue_start + queue_len) % QUEUE_CAP] = item;
  queue_len++;
}

static Item peek() {
  return queue[queue_start];
}

static Item dequeue() {
  if (queue_len <= 0) {
    return;
  }
  Item result = peek();
  queue_len--;
  queue_start++;
  queue_start %= QUEUE_CAP;
  return result;
}

void queue_tone(int tone_millis, int quiet_millis, int repeats = 1, bool flush = false) {
  unsigned long now_time = millis();
  if (next_time < now_time || flush) {
    next_time = now_time;
  }
  if (flush) {
	  queue_len = 0;
  }
  
  for (int i = 0; i < repeats; i++) {
    Item start {next_time, HIGH};
    next_time += tone_millis;
    Item end {next_time, LOW};
    next_time += quiet_millis;

    enqueue(start);
    enqueue(end);
  }
}

void set_ticking(bool ticking) {
	is_ticking = ticking;
}


void sound_setup() {
  pinMode(soundPin, OUTPUT);
}

void sound_loop() {
  while (queue_len > 0 && peek().time <= millis()) {
    digitalWrite(soundPin, dequeue().val);
  }
  if (queue_len == 0 && is_ticking) {
	  digitalWrite(soundPin, millis() % 500 < 5 ? HIGH : LOW);
  }
  if (queue_len == 0 && !is_ticking) {
	  digitalWrite(soundPin, LOW);
  }
}

