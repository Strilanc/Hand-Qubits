//
// Manages one of the hardware timer interrupts.
//

#ifndef HARDWARE_TIMER_1_H
#define HARDWARE_TIMER_1_H

void start_periodic_interrupt_timer_1(long microseconds, void(*callback)());
void stop_periodic_interrupt_timer_1();

#endif
