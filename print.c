#include "traceroute.h"

// Import global flags
extern struct s_flags g_flags;


////////////////////////////////
// Stderr
////////////////////////////////

// Print start message
void print_init (struct sockaddr_in addr)
{
    // Resolve hostname
    char ipbuf[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, ipbuf, sizeof(ipbuf));

    // Print init
    fprintf(stderr, "ft_traceroute to %s (%s), %d hops max, %lu byte packets",
        g_flags.host, ipbuf, g_flags.max_ttl, sizeof(struct trace_packet));
    fflush(stdout);
}

// Print help message
void print_help (void)
{
    fprintf(stderr, "Version 1.0.0+Darwin\n");
    fprintf(stderr, "Usage: ft_traceroute [-f first_ttl] [-m max_ttl]\n");
    fprintf(stderr, "        [-q nqueries] [-w waittime] [-z pausemsecs] host\n");
}

// Print bad value error
void print_bad_value (const char *opt_name, const char *value)
{
    fprintf(stderr, "ft_traceroute: \"%s\" bad value for %s\n", value, opt_name);
}

// Print out of bounds error
void print_oob (const char *opt_name, const char *operator, int bound)
{
    fprintf(stderr, "ft_traceroute: %s must be %s %d\n", opt_name, operator, bound);
}

// Print host too long error
void print_long_host (const char *host)
{
    fprintf(stderr, "ft_traceroute: hostname \"%.32s...\" is too long\n", host);
}

// Print ttl flags error
void print_ttl_issue (int first_ttl, int max_ttl)
{
    fprintf(stderr, "ft_traceroute: first ttl (%d) may not be greater than max ttl (%d)\n", first_ttl, max_ttl);
}

////////////////////////////////
// Stdout
////////////////////////////////

// Print new line with ttl
void print_newttl (int ttl)
{
    printf("\n%*d  ", g_flags.padlen, ttl);
}

// Print newline for formatting
void print_newline (void)
{
    printf("\n");
    printf("%*s", g_flags.padlen + 2, "");
}

// Print host and ip address
void print_address (struct in_addr addr)
{
    printf("%s (%s)  ", inet_ntoa(addr), inet_ntoa(addr));
}

// Print wildcard when no reply
void print_star (void)
{
    printf("*  ");
    fflush(stdout);
}

// Print time of round trip
void print_timestamp (struct timeval old, struct timeval new)
{
    unsigned int t_diff = timediff(old, new);
    printf("%d.%d ms  ", t_diff / 1000, t_diff % 1000);
    fflush(stdout);
}
