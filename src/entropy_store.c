#include <stdint.h>

extern uint32_t fetch(void);
uint32_t s_value = 0;
uint32_t s_range = 1;
const uint32_t N = 1<<31;

uint32_t generate_uniform32(uint32_t n) {
  for (;;) {
    // Preload entropy one bit at a time into s
    while (s_range < N) {
      s_value <<= 1;
      s_value |= fetch();
      s_range <<= 1;
    }
    // Sample entropy s to a multiple of n
    uint32_t r = s_range / n;
    uint32_t c = s_range % n;
    if (s_value >= c) {
      // Sample successful
      s_value -= c;
      uint32_t a = s_value / n;
      uint32_t b = s_value % n;
      s_value = a;
      s_range = r;
      return b;
    } else {
      // Sample unsuccessful
      s_range = c;
    }
  }
}