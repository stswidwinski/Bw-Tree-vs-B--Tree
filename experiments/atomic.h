/// @file
/// @author Alexander Thomson <thomson@cs.yale.edu>
///
/// Modified by Christina Wallin <christina.wallin@yale.edu>
///
/// Single-threaded performance (tick.zoo.cs.yale.edu, 12/11/11):
///
///    AtomicQueue<int>:
///      Push/Pop: 90.2 ns
///
///
/// Cut down and changed for final project: Stan Swidwinski

#ifndef _DB_UTILS_ATOMIC_H_
#define _DB_UTILS_ATOMIC_H_

#include <queue>

#include <assert.h>
#include "experiments/mutex.h"

using std::queue;

/// @class AtomicQueue<T>
///
/// Queue with atomic push and pop operations.
///
/// @TODO(alex): This should use lower-contention synchronization.
template<typename T>
class AtomicQueue {
 public:
  AtomicQueue() {}

  // Returns the number of elements currently in the queue.
  int Size() {
    mutex_.Lock();
    int size = queue_.size();
    mutex_.Unlock();
    return size;
  }

  // Atomically pushes 'item' onto the queue.
  void Push(const T& item) {
    mutex_.Lock();
    queue_.push(item);
    mutex_.Unlock();
  }

  // If the queue is non-empty, (atomically) sets '*result' equal to the front
  // element, pops the front element from the queue, and returns true,
  // otherwise returns false.
  bool Pop(T* result) {
    mutex_.Lock();
    if (!queue_.empty()) {
      *result = queue_.front();
      queue_.pop();
      mutex_.Unlock();
      return true;
    } else {
      mutex_.Unlock();
      return false;
    }
  }

  // If mutex is immediately acquired, pushes and returns true, else immediately
  // returns false.
  bool PushNonBlocking(const T& item) {
    if (mutex_.TryLock()) {
      queue_.push(item);
      mutex_.Unlock();
      return true;
    } else {
      return false;
    }
  }

  // If mutex is immediately acquired AND queue is nonempty, pops and returns
  // true, else returns false.
  bool PopNonBlocking(T* result) {
    if (mutex_.TryLock()) {
      if (!queue_.empty()) {
        *result = queue_.front();
        queue_.pop();
        mutex_.Unlock();
        return true;
      } else {
        mutex_.Unlock();
        return false;
      }
    } else {
      return false;
    }
  }

 private:
  queue<T> queue_;
  Mutex mutex_;
};

#endif  // _DB_UTILS_ATOMIC_H_

