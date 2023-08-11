#include "traceroute.h"


// Return timeval struct with current time
struct timeval getnow (void)
{
    struct timeval now;

    gettimeofday(&now, 0);
    return now;
}

// Return usec diff between two timeval structs
unsigned int timediff (struct timeval old, struct timeval new)
{
    return (new.tv_sec - old.tv_sec) * 1000000 + new.tv_usec - old.tv_usec;
}

// Wait for x msecs
void waitfor (unsigned int msecs)
{
    struct timeval start = getnow();

    while (timediff(start, getnow()) < msecs * 1000)
        ;
}
