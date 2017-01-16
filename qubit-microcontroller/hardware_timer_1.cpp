#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "hardware_timer_1.h"

static void (*isrCurrentCallback)();

// Interrupt Service Routine. Entrypoint for timer interrupts.
ISR(TIMER1_OVF_vect) {
  isrCurrentCallback();
}

unsigned char period_to_clock_select_bits(long microseconds, long& cycles) {
  const long MAX_CYCLES = 65535;
  cycles = (F_CPU / 2000000) * microseconds;

  if (cycles <= MAX_CYCLES) {
    return _BV(CS10); // No prescale.
  }

  cycles >>= 3;
  if(cycles <= MAX_CYCLES) {
    return _BV(CS11); // 8
  }
  
  cycles >>= 3;
  if(cycles <= MAX_CYCLES) {
    return _BV(CS11) | _BV(CS10); // 64
  }
  
  cycles >>= 2;
  if(cycles <= MAX_CYCLES) {
    return _BV(CS12); // 256
  }

  cycles >>= 2;
  cycles = min(cycles, MAX_CYCLES);
  return _BV(CS12) | _BV(CS10); // 1024
}

void start_periodic_interrupt_timer_1(long microseconds, void (*callback)()) {
  isrCurrentCallback = callback;
 
  long cycles;
  unsigned char clockSelectBits = period_to_clock_select_bits(microseconds, cycles);
  
  // Disable interrupts, to avoid torn register writes.
  cli();

  // Setup registers.
  TCCR1A = 0;
  TCCR1B = _BV(WGM13) | clockSelectBits;
  ICR1 = cycles;
  TIMSK1 = _BV(TOIE1);

  // Restore interrupts.
  sei();
}

void stop_periodic_interrupt_timer_1() {
  // Clear clock select bits.
  TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
}
