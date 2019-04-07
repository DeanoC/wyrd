#include "core/core.h"
#include "theforge/renderer.hpp"
#include "utils.hpp"

namespace TheForge {

void AllocatorUint32ToStr(char *outStr, size_t strLen, uint32_t num) {
  char *p;         /* pointer to traverse string */
  char *firstdig;  /* pointer to first digit */
  char temp;      /* temp char */
  uint32_t val = num; /* full value */
  uint32_t digval;    /* value of digit */

  p = outStr;
  firstdig = p; /* save pointer to first digit */
  do {
    digval = (uint32_t) (num % 10UL);
    num /= 10UL; /* get next digit */

    /* convert to ascii and store */
    if (digval > 9) {
      *p++ = (char) (digval - 10 + 'a'); /* a letter */
    } else {
      *p++ = (char) (digval + '0');
    } /* a digit */
  } while (val > 0);

  /* We now have the digit of the number in the buffer, but in reverse
	 order.  Thus we reverse them now. */
  *p-- = '\0'; /* terminate string; p points to last digit */
  do {
    temp = *p;
    *p = *firstdig;
    *firstdig = temp; /* swap *p and *firstdig */
    --p;
    ++firstdig;         /* advance to next two digits */
  } while (firstdig < p); /* repeat until halfway */
}

void AllocatorUint64ToStr(char *outStr, size_t strLen, uint64_t num) {
  char *p;         /* pointer to traverse string */
  char *firstdig;  /* pointer to first digit */
  char temp;      /* temp char */
  uint64_t val = num; /* full value */
  unsigned long long digval;    /* value of digit */

  p = outStr;
  firstdig = p; /* save pointer to first digit */
  do {
    digval = (uint64_t) (val % 10ULL);
    val /= 10ULL; /* get next digit */

  /* convert to ascii and store */
    if (digval > 9) {
      *p++ = (char) (digval - 10 + 'a'); /* a letter */
    } else {
      *p++ = (char) (digval + '0');
    } /* a digit */
  } while (val > 0);

  /* We now have the digit of the number in the buffer, but in reverse
   order.  Thus we reverse them now. */
  *p-- = '\0'; /* terminate string; p points to last digit */
  do {
    temp = *p;
    *p = *firstdig;
    *firstdig = temp; /* swap *p and *firstdig */
    --p;
    ++firstdig;         /* advance to next two digits */
  } while (firstdig < p); /* repeat until halfway */
}


}

