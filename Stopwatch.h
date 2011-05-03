#ifndef __STOPWATCH_H__
#define __STOPWATCH_H__

#include <sys/time.h>

#define US_PER_SEC 1000000

// class Stopwatch
//
// Class that provides time differences (wall clock time) accurate
// to the microsecond.
class Stopwatch {
 public:
  // Default constructor
  Stopwatch();

  // Starts the stopwatch.  Each time this is called, the stopwatch
  // is reset.
  void start();

  // Returns the amount of wall clock time elapsed since start() was
  // called.  If start was never called, returns 0 seconds and 0
  // microseconds.
  timeval timeElapsed();

  // Returns the amount of wall clock time elapsed since start() was
  // called.  Time returned is 
  // 
  // 
  float secondsElapsed();

 private:
  int timeval_subtract (timeval &result, timeval t1, timeval t0);

  int     _isStarted;
  timeval _startTime;
};

#endif
