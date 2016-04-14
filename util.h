/*
  util.h           Supplied by Jose Faleiro

  We only really care about cmp_and_swap function. 
*/

#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

inline void
do_pause()
{
  asm volatile("pause;":::);
}

inline void
barrier() {
  asm volatile("":::"memory");
}

// An indivisible unit of work. 
inline void
single_work() 
{
  asm volatile("nop;":::"memory");
}


inline bool
cmp_and_swap(volatile uint64_t *to_write,
             uint64_t to_cmp,
             uint64_t new_value) {
  volatile uint64_t out;
  asm volatile("lock; cmpxchgq %2, %1"
               : "=a" (out), "+m"(*to_write)
               : "q" (new_value), "0"(to_cmp));
  return out == to_cmp;
}

inline uint64_t
xchgq(volatile uint64_t *addr, uint64_t new_val)
{
  uint64_t result;

  // The + in "+m" denotes a read-modify-write operand.
  asm volatile("lock; xchgq %0, %1" :
               "+m" (*addr), "=a" (result) :
               "1" (new_val) :
               "cc");
  return result;
}

inline void
reentrant_lock(volatile uint64_t *word, uint32_t threadId) {
  uint64_t mask = 0xFFFFFFFF00000000;
  barrier();
  if (((*word & mask) >> 32) != threadId) {
    barrier();
    while (true) {
      if ((*word == 0) && cmp_and_swap(word, 0, ((uint64_t)threadId<<32) | 1)) {
        break;
      }
      do_pause();
    }
  }
  else {
    *word += 1;
  }
}

inline void
reentrant_unlock(volatile uint64_t *word) {
  uint64_t mask = 0x00000000FFFFFFFF;
  *word -= 1;
  if ((*word & mask) == 0) {
    xchgq(word, 0);
  }
}

inline bool
try_lock(volatile uint64_t *word)
{
        return cmp_and_swap(word, 0, 1);
}

// Spin lock implementation. XXX: Is test-n-test-n-set better?
inline void
lock(volatile uint64_t *word) {
        while (true) {
                if ((*word == 0) && (xchgq(word, 1) == 0)) {
                        break;
                } 
                do_pause();
        }
}

inline void
unlock(volatile uint64_t *word) {
  xchgq(word, 0);
}

inline uint32_t
fetch_and_increment_32(volatile uint32_t *variable)
{
        int counter_value = 1;
        asm volatile ("lock; xaddq %%rax, %1;"
                      : "=a" (counter_value), "+m" (*variable)
                      : "a" (counter_value)
                      : "memory");
        return counter_value + 1;
}

inline uint64_t
fetch_and_increment(volatile uint64_t *variable)
{
  long counter_value = 1;
  asm volatile ("lock; xaddq %%rax, %1;"
                : "=a" (counter_value), "+m" (*variable)
                : "a" (counter_value)
                : "memory");
  return counter_value + 1;
}

inline long
fetch_and_decrement(volatile uint64_t *variable) 
{
  long counter_value = -1;
  asm volatile ("lock; xaddq %%rax, %1;"
                : "=a" (counter_value), "+m" (*variable)
                : "a" (counter_value)
                : "memory");
  return counter_value - 1;
}    



// Use this function to read the timestamp counter. 
// Don't bother with using serializing instructions like cpuid and others,
// found that it works well without them. 
inline uint64_t
rdtsc()
{
  uint32_t cyclesHigh, cyclesLow;
  asm volatile("rdtsc\n\t"
               "movl %%edx, %0\n\t"
               "movl %%eax, %1\n\t"
               : "=r" (cyclesHigh), "=r" (cyclesLow) ::
                 "%rax", "%rdx");
  return (((uint64_t)cyclesHigh<<32) | cyclesLow);
}

// Check the amount of time (in cycles) required to perform an indivisible
// unit of work. 
// Measured 2 cycles on morz and smorz. 
inline double
check_pause()
{
  int i;
  uint64_t start_time, end_time;
  start_time = rdtsc();
  for (i = 0; i < 200; ++i) {
    do_pause();
  }
  end_time = rdtsc();
  return ((double)end_time - (double)start_time) / 200.0;
}

// Measure rdtsc overhead. 
inline double
check_rdtsc()
{
  int i;
  uint64_t start_time, end_time;
  start_time = rdtsc();
  for (i = 0; i < 10000; ++i) {
    end_time = rdtsc();
  }
        
  return (end_time - start_time) / 10000.0;
}

#endif //UTIL_H
