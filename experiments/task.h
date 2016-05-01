/// @file
/// @author Alexander Thomson <thomson@cs.yale.edu>
///
/// Reused class from assigment 2. Used for benchmarking purposes.

#ifndef _DB_UTILS_TASK_H_
#define _DB_UTILS_TASK_H_

#include <cassert>
#include <queue>
#include <string>
#include <vector>
#include <typeinfo>

using std::queue;
using std::string;
using std::vector;

/// @class Unit
///
/// A dummy type used as the default arg type for Function and Method.
class Unit {};

/// @class Task
///
/// Tasks represent arbitrary computations. A class typically consists of a
/// pointer to a function and a set of args. Running the task involves calling
/// the function with the provided args.
class Task {
 public:
  virtual ~Task() {}

  // Run the task.
  virtual void Run() = 0;
};

/// @class RTask<R>
///
/// All tasks have the (possibly void) return types of their associated
/// functions. This subclass of Task is parametrized over return type but not
/// arg type.
template<typename R>
class RTask : public Task {
 public:
  virtual ~RTask() {}
  virtual void Run() = 0;

  /// Sets a location for the return value to be stored.
  /// Requires:
  virtual void SetResultPointer(R* r) = 0;
};

/// @class Method<T, R, A, B, C, D, E>
///
template<class T,
         typename R,
         typename A = Unit,
         typename B = Unit,
         typename C = Unit,
         typename D = Unit,
         typename E = Unit>
class Method : public RTask<R> {
 public:
  // For nonvoid methods a pointer to where to put the return value must be
  // provided.
  Method(T* t, R (T::*f)(), R* r);
  Method(T* t, R (T::*f)(A), R* r, A a);
  Method(T* t, R (T::*f)(A, B), R* r, A a, B b);
  Method(T* t, R (T::*f)(A, B, C), R* r, A a, B b, C c);
  Method(T* t, R (T::*f)(A, B, C, D), R* r, A a, B b, C c, D d);
  Method(T* t, R (T::*f)(A, B, C, D, E), R* r, A a, B b, C c, D d, E e);

  // For void methods, no return value pointer is provided.
  Method(T* t, void (T::*f)());
  Method(T* t, void (T::*f)(A), A a);
  Method(T* t, void (T::*f)(A, B), A a, B b);
  Method(T* t, void (T::*f)(A, B, C), A a, B b, C c);
  Method(T* t, void (T::*f)(A, B, C, D), A a, B b, C c, D d);
  Method(T* t, void (T::*f)(A, B, C, D, E), A a, B b, C c, D d, E e);

  // Runs the method. For nonvoid methods, sets *r_ equal to the return value.
  virtual void Run();

  virtual void SetResultPointer(R* r) { r_ = r; }

  void changeParam(A a);
  void changeParam(B b);
  void changeParam(C c);

 private:
  // Number of args.
  int args_;

  // Class instance on which to call the method.
  T* t_;

  // Method to be called.
  R (T::*f0_)();
  R (T::*f1_)(A);
  R (T::*f2_)(A, B);
  R (T::*f3_)(A, B, C);
  R (T::*f4_)(A, B, C, D);
  R (T::*f5_)(A, B, C, D, E);

  // Pointer to where to put the return value.
  R* r_;

  // Args to use at run time.
  A a_;
  B b_;
  C c_;
  D d_;
  E e_;
};


// Method

template<typename R>
class RunMethodTemplated {
 public:
  template<class T>
  static inline void Run(T* t, R (T::*f)(), R* r);
  template<class T, typename A>
  static inline void Run(T* t, R (T::*f)(A), R* r, A a);
  template<class T, typename A, typename B>
  static inline void Run(T* t, R (T::*f)(A, B), R* r, A a, B b);
  template<class T, typename A, typename B, typename C>
  static inline void Run(T* t, R (T::*f)(A, B, C), R* r, A a, B b, C c);
  template<class T, typename A, typename B, typename C, typename D>
  static inline void Run(T* t, R (T::*f)(A, B, C, D), R* r, A a, B b, C c, D d);
  template<class T, typename A, typename B, typename C, typename D, typename E>
  static inline void Run(T* t, R (T::*f)(A, B, C, D, E), R* r, A a, B b, C c, D d, E e);
};

template<typename R> template<class T>
inline void RunMethodTemplated<R>::Run(T* t, R (T::*f)(), R* r) { *r = (t->*f)(); }
template<typename R> template<class T, typename A>
inline void RunMethodTemplated<R>::Run(T* t, R (T::*f)(A), R* r, A a) { *r = (t->*f)(a); }
template<typename R> template<class T, typename A, typename B>
inline void RunMethodTemplated<R>::Run(T* t, R (T::*f)(A, B), R* r, A a, B b) { *r = (t->*f)(a, b); }
template<typename R> template<class T, typename A, typename B, typename C>
inline void RunMethodTemplated<R>::Run(T* t, R (T::*f)(A, B, C), R* r, A a, B b, C c) { *r = (t->*f)(a, b, c); }
template<typename R> template<class T, typename A, typename B, typename C, typename D>
inline void RunMethodTemplated<R>::Run(T* t, R (T::*f)(A, B, C, D), R* r, A a, B b, C c, D d) { *r = (t->*f)(a, b, c, d); }
template<typename R> template<class T, typename A, typename B, typename C, typename D, typename E>
inline void RunMethodTemplated<R>::Run(T* t, R (T::*f)(A, B, C, D, E), R* r, A a, B b, C c, D d, E e) { *r = (t->*f)(a, b, c, d, e); }

template<> template<class T>
inline void RunMethodTemplated<void>::Run(T* t, void (T::*f)(), void* r) { (t->*f)(); }
template<> template<class T, typename A>
inline void RunMethodTemplated<void>::Run(T* t, void (T::*f)(A), void* r, A a) { (t->*f)(a); }
template<> template<class T, typename A, typename B>
inline void RunMethodTemplated<void>::Run(T* t, void (T::*f)(A, B), void* r, A a, B b) { (t->*f)(a, b); }
template<> template<class T, typename A, typename B, typename C>
inline void RunMethodTemplated<void>::Run(T* t, void (T::*f)(A, B, C), void* r, A a, B b, C c) { (t->*f)(a, b, c); }
template<> template<class T, typename A, typename B, typename C, typename D>
inline void RunMethodTemplated<void>::Run(T* t, void(T::*f)(A, B, C, D), void* r, A a, B b, C c, D d) { (t->*f)(a, b, c, d); }
template<> template<class T, typename A, typename B, typename C, typename D, typename E>
inline void RunMethodTemplated<void>::Run(T* t, void (T::*f)(A, B, C, D, E), void* r, A a, B b, C c, D d, E e) { (t->*f)(a, b, c, d, e); }

template<class T, typename R, typename A, typename B, typename C, typename D, typename E>
Method<T, R, A, B, C, D, E>::Method(T* t, R (T::*f)(), R* r)
    : args_(0), t_(t), f0_(f), r_(r) {}
template<class T, typename R, typename A, typename B, typename C, typename D, typename E>
Method<T, R, A, B, C, D, E>::Method(T* t, R (T::*f)(A), R* r, A a)
    : args_(1), t_(t), f1_(f), r_(r), a_(a) {}
template<class T, typename R, typename A, typename B, typename C, typename D, typename E>
Method<T, R, A, B, C, D, E>::Method(T* t, R (T::*f)(A, B), R* r, A a, B b)
    : args_(2), t_(t), f2_(f), r_(r), a_(a), b_(b) {}
template<class T, typename R, typename A, typename B, typename C, typename D, typename E>
Method<T, R, A, B, C, D, E>::Method(T* t, R (T::*f)(A, B, C), R* r, A a, B b, C c)
    : args_(3), t_(t), f3_(f), r_(r), a_(a), b_(b), c_(c) {}
template<class T, typename R, typename A, typename B, typename C, typename D, typename E>
Method<T, R, A, B, C, D, E>::Method(T* t, R (T::*f)(A, B, C, D), R* r, A a, B b, C c, D d)
    : args_(4), t_(t), f4_(f), r_(r), a_(a), b_(b), c_(c), d_(d) {}
template<class T, typename R, typename A, typename B, typename C, typename D, typename E>
Method<T, R, A, B, C, D, E>::Method(T* t, R (T::*f)(A, B, C, D, E), R* r, A a, B b, C c, D d, E e)
    : args_(5), t_(t), f5_(f), r_(r), a_(a), b_(b), c_(c), d_(d), e_(e) {}

template<class T, typename R, typename A, typename B, typename C, typename D, typename E>
Method<T, R, A, B, C, D, E>::Method(T* t, void (T::*f)())
    : args_(0), t_(t), f0_(f), r_(NULL) {}
template<class T, typename R, typename A, typename B, typename C, typename D, typename E>
Method<T, R, A, B, C, D, E>::Method(T* t, void (T::*f)(A), A a)
    : args_(1), t_(t), f1_(f), r_(NULL), a_(a) {}
template<class T, typename R, typename A, typename B, typename C, typename D, typename E>
Method<T, R, A, B, C, D, E>::Method(T* t, void (T::*f)(A, B), A a, B b)
    : args_(2), t_(t), f2_(f), r_(NULL), a_(a), b_(b) {}
template<class T, typename R, typename A, typename B, typename C, typename D, typename E>
Method<T, R, A, B, C, D, E>::Method(T* t, void (T::*f)(A, B, C), A a, B b, C c)
    : args_(3), t_(t), f3_(f), r_(NULL), a_(a), b_(b), c_(c) {}
template<class T, typename R, typename A, typename B, typename C, typename D, typename E>
Method<T, R, A, B, C, D, E>::Method(T* t, void (T::*f)(A, B, C, D), A a, B b, C c, D d)
    : args_(4), t_(t), f4_(f), r_(NULL), a_(a), b_(b), c_(c), d_(d) {}
template<class T, typename R, typename A, typename B, typename C, typename D, typename E>
Method<T, R, A, B, C, D, E>::Method(T* t, void (T::*f)(A, B, C, D, E), A a, B b, C c, D d, E e)
    : args_(5), t_(t), f5_(f), r_(NULL), a_(a), b_(b), c_(c), d_(d), e_(e) {}

template<class T, typename R, typename A, typename B, typename C, typename D, typename E>
void Method<T, R, A, B, C, D, E>::Run() {
  switch (args_) {
    case 0: RunMethodTemplated<R>::Run(t_, f0_, r_); break;
    case 1: RunMethodTemplated<R>::Run(t_, f1_, r_, a_); break;
    case 2: RunMethodTemplated<R>::Run(t_, f2_, r_, a_, b_); break;
    case 3: RunMethodTemplated<R>::Run(t_, f3_, r_, a_, b_, c_); break;
    case 4: RunMethodTemplated<R>::Run(t_, f4_, r_, a_, b_, c_, d_); break;
    case 5: RunMethodTemplated<R>::Run(t_, f5_, r_, a_, b_, c_, d_, e_); break;
  }
}

template<class T, typename R, typename A, typename B, typename C, typename D, typename E>
void Method<T, R, A, B, C, D, E>::changeParam(A a) {a_ = a;}
template<class T, typename R, typename A, typename B, typename C, typename D, typename E>
void Method<T, R, A, B, C, D, E>::changeParam(B b) {b_ = b;}
template<class T, typename R, typename A, typename B, typename C, typename D, typename E>
void Method<T, R, A, B, C, D, E>::changeParam(C c) {c_ = c;}

#endif