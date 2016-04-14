/*
  util.cc						Supplied by Jose Faleiro

  We only really care about cmp_and_swap function. 
*/

#include <util.h>

extern inline void do_pause();

extern inline void barrier();

extern inline bool cmp_and_swap(volatile uint64_t *to_write, uint64_t to_cmp,
                         uint64_t new_value);

extern inline uint64_t xchgq(volatile uint64_t *addr, uint64_t new_val);

extern inline void reentrant_lock(volatile uint64_t *word, uint32_t threadId);

extern inline void reentrant_unlock(volatile uint64_t *word);

extern inline void lock(volatile uint64_t *word);

extern inline void unlock(volatile uint64_t *word);

extern inline uint32_t fetch_and_increment_32(volatile uint32_t *variable);

extern inline uint64_t fetch_and_increment(volatile uint64_t *variable);

extern inline long fetch_and_decrement(volatile uint64_t *variable);

extern inline void single_work();

// Use this function to read the timestamp counter. 
// Don't bother with using serializing instructions like cpuid and others,
// found that it works well without them. 
extern inline uint64_t rdtsc();

// Check the amount of time (in cycles) required to perform an indivisible
// unit of work. 
// Measured 2 cycles on morz and smorz. 
extern inline double check_pause();

// Measure rdtsc overhead. 
extern inline double check_rdtsc();
