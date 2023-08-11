
#ifndef FT_TRACEROUTE_H
# define FT_TRACEROUTE_H

// Std
# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include <string.h>

// Time, types, math
# include <sys/time.h>
# include <stdint.h>
# include <math.h>

// Networking
# include <netdb.h>
# include <arpa/inet.h>

// Errors & signals
# include <signal.h>
# include <errno.h>


// Flag struct
struct s_flags {
    int     first_ttl;      // -f | Starting ttl
    int     max_ttl;        // -m | Maximum ttl
    int     nqueries;       // -q | Number of probes sent per ttl
    int     waittime;       // -w | Timeout for response (seconds)
    int     pausemsecs;     // -z | Wait time between probes (milliseconds)
    int     padlen;         // Padding length (depends on max_ttl)
    int     finished;       // Indicates the program should terminate
    char    host[65];       // A copy of the host argv
};

// Packet structure
// struct ppacket {
//     uint8_t     type;           // 8 for request, 0 for reply
//     uint8_t     code;           // Always 0
//     uint16_t    checksum;       // Computed total for checks
//     uint16_t    identifier;     // Unique ID of current ping "loop" (always 0 here)
//     uint16_t    sequence_number;// Unique ID of ping in loop (starts at 1, increments)
//     char        time[8];        // Timestamp as data optimization
//     char        data[48];       // Data (static)
// };

// Trace packet struct for send()
struct trace_packet {
    uint16_t    srcport;        // Source port
    uint16_t    dstport;        // Destination port
    uint16_t    len;            // Packet length
    uint16_t    checksum;       // Computed total for checks (unused if 0)
    char        data[24];       // Data (static)
};

// Trace packet defaults
# define SRCPORT_DEFAULT 34558
# define DSTPORT_DEFAULT 33434
# define CHECKSUM_DEFAULT 0


// Info about previous recv() struct
struct sprev {
    struct in_addr  ip;         // Store IP (to check if new)
    struct timeval  sent;       // Store timestamp of send (for timediff)
    int             sameline;   // Mark if newline (for formatting)
};

// recv() packet indexes
# define ICMP_TYPE 20
# define ICMP_CODE 21

// recv() packet - ongoing traceroute values
# define TTL_EXCEEDED 11
# define TTL_EXCEEDED_TRANSIT 0

// recv() packet - finished traceroute values
# define DESTINATION_UNREACHABLE 3
# define PORT_UNREACHABLE 3


// Option functions
void parse_opt (char **args);

// Print functions
void print_init (struct sockaddr_in addr);
void print_help (void);
void print_bad_value (const char *opt_name, const char *value);
void print_oob (const char *opt_name, const char *operator, int bound);
void print_long_host (const char *host);
void print_ttl_issue (int first_ttl, int max_ttl);
void print_newttl (int ttl);
void print_newline (void);
void print_address (struct in_addr addr);
void print_star (void);
void print_timestamp (struct timeval old, struct timeval new);

// Time functions
struct timeval getnow (void);
unsigned int timediff (struct timeval old, struct timeval new);
void waitfor (unsigned int msecs);

// Util functions
uint16_t byteswapped (uint16_t n);
void memcopy (void *dst, void *src, size_t n);
void memzero (void *ptr, size_t n);
int memcomp (void *l, void *r, size_t n);
int str_is (const char *a, const char *b);
int str_is_num (const char *s);
int len (const char *s);
int toint (const char *s);
size_t nbrlen (int n);

#endif
