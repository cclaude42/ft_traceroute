#include "traceroute.h"


// Returns a uint16_t with its two bytes swapped
uint16_t byteswapped (uint16_t n)
{
    return (n << 8) + (n >> 8);
}

////////////////////////////////
// Memory
////////////////////////////////

// Analogous to memcpy
void memcopy (void *dst, void *src, size_t n)
{
    for (size_t i = 0 ; i < n ; i++)
        ((char *)dst)[i] = ((char *)src)[i];
}

// Analogous to bzero
void memzero (void *ptr, size_t n)
{
    for (size_t i = 0 ; i < n ; i++)
        ((char *)ptr)[i] = 0;
}

// Analogous to memcmp
int memcomp (void *l, void *r, size_t n)
{
    for (size_t i = 0 ; i < n ; i++)
        if (((char *)l)[i] != ((char *)r)[i])
            return ((char *)l)[i] - ((char *)r)[i];
    return 0;
}


////////////////////////////////
// Strings
////////////////////////////////

// Analogous to reverse strcmp
int str_is (const char *a, const char *b)
{
    size_t i;

    for (i = 0 ; a[i] && b[i] ; i++) {
        if (a[i] != b[i]) {
            return 0;
        }
    }
    return a[i] == b[i];
}

// Check is s is numeric
int str_is_num (const char *s)
{
    if (*s == '-')
        s++;
    while (*s) {
        if (*s < '0' || *s > '9') {
            return 0;
        }
        s++;
    }
    return 1;
}

// Analogous to strlen
int len (const char *s)
{
    size_t i = 0;

    while (s[i])
        ++i;
    return i;
}


////////////////////////////////
// Integers
////////////////////////////////

// Calculates number length
size_t nbrlen (int n)
{
    size_t l = 1;

    while (n > 9) {
        l++;
        n /= 10;
    }

    return l;
}

// Analogous to atoi
int toint (const char *str)
{
    int s = 1;
    long long n = 0;

    if (*str == '-') {
        s = -1;
        str++;
    }

    while (*str) {
        n = n * 10 + (*str - '0');
        str++;
    }

    return n * s;
}