#include "Stopwatch.h"

// Initializes the stopwatch object
Stopwatch::Stopwatch()
{
  _isStarted = 0;
  _startTime.tv_sec  = 0;
  _startTime.tv_usec = 0;
}

void Stopwatch::start()
{
  gettimeofday(&_startTime, 0);  
  _isStarted = 1;
}

timeval Stopwatch::timeElapsed()
{
  timeval zeroTime;
  timeval currentTime;
  timeval elapsedTime;
  zeroTime.tv_sec  = 0;
  zeroTime.tv_usec = 0;

  gettimeofday(&currentTime, 0);
  if (!_isStarted || timeval_subtract(elapsedTime, currentTime, _startTime)) {
    return zeroTime;
  }
  else {
    return elapsedTime;
  }
}

float Stopwatch::secondsElapsed() 
{
  timeval elapsed = timeElapsed();
  return elapsed.tv_sec + elapsed.tv_usec / (float)US_PER_SEC;
}


// Helper function to find differences in time, accurate to the
// microsecond. Return 1 if result is negative
int Stopwatch::timeval_subtract (timeval &result, timeval t1, timeval t0)
{
  // perform the carry for the later subtraction by updating y
  if (t1.tv_usec < t0.tv_usec) {
    int nsec = (t0.tv_usec - t1.tv_usec) / 1000000 + 1;
    t0.tv_usec -= 1000000 * nsec;
    t0.tv_sec += nsec;
  }
  if (t1.tv_usec - t0.tv_usec > 1000000) {
    int nsec = (t1.tv_usec - t0.tv_usec) / 1000000;
    t0.tv_usec += 1000000 * nsec;
    t0.tv_sec -= nsec;
  }
  
  // Compute the time remaining to wait.  tv_usec is certainly positive
  result.tv_sec = t1.tv_sec - t0.tv_sec;
  result.tv_usec = t1.tv_usec - t0.tv_usec;

  // return 1 if result is negative
  return t1.tv_sec < t0.tv_sec;
}
