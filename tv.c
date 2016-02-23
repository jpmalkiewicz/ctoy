#include <sys/time.h>

#include "tv.h"

void tvdiff(struct timeval *td, const struct timeval *t1, const struct timeval *t2)
{
    if (t1->tv_usec > t2->tv_usec) {
        td->tv_usec = t1->tv_usec - t2->tv_usec;
        td->tv_sec = t1->tv_sec - t2->tv_sec;
    } else {
        td->tv_usec = 1000000 + t1->tv_usec - t2->tv_usec;
        td->tv_sec = t1->tv_sec - t2->tv_sec - 1;
    }
}
