/// @file
/// @author Alexander Thomson <thomson@cs.yale.edu>
///
/// Single-threaded performance (tick.zoo.cs.yale.edu, 12/11/11):
///
///    Mutex:
///      Lock/Unlock: 20.4 ns
///    MutexRW:
///      ReadLock/Unlock: 34.8 ns
///      WriteLock/Unlock: 33.2 ns
///
/// Shortened and changed for final project: Stan Swidwinski

#ifndef _DB_UTILS_MUTEX_H_
#define _DB_UTILS_MUTEX_H_

#include <pthread.h>

/// @class Mutex
///
/// A simple mutex, actually a thin wrapper around pthread's mutex
/// implementation.
class Mutex {
 public:
  /// Mutexes come into the world unlocked.
  Mutex() {
    pthread_mutex_init(&mutex_, NULL);
  }

  /// Locks a mutex. Blocks until the mutex has been successfully acquired.
  inline void Lock() {
    pthread_mutex_lock(&mutex_);
  }

  /// Attempts to lock a mutex. If the mutex is not already locked, locks the
  /// mutex and returns true, else returns false.
  inline bool TryLock() {
    return pthread_mutex_trylock(&mutex_) == 0;
  }

  /// Releases an already held lock on a mutex.
  ///
  /// Requires: A lock is already held on the mutex.
  inline void Unlock() {
    pthread_mutex_unlock(&mutex_);
  }

 private:
  friend class Condition;

  // Actual pthread mutex wrapped by Mutex class.
  pthread_mutex_t mutex_;
};

#endif  // _DB_UTILS_MUTEX_H_

