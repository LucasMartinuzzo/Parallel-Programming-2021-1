#define _POSIX_C_SOURCE 199309L

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static double rtclock() {
  struct timespec t;
  clock_gettime(CLOCK_REALTIME, &t);
  return t.tv_sec + t.tv_nsec * 1e-9;
}

...

int main(int argc, char *argv[]) {
  double a = rtclock();
  init(argc, argv);
  allocate_image_buffer();
  double b = rtclock();
  compute_mandelbrot();
  double c = rtclock();
  write_to_file();
  double d = rtclock();

  printf("%lf, %lf, %lf\n", (b - a) + (d - c), c - b, d - a);
  return 0;
};
