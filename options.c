#include "traceroute.h"

// Import global flags
extern struct s_flags g_flags;


// Set flag
void set_opt (const char *opt_name, int *opt, const char *value, int min, int max)
{
    // Check value
    if (!str_is_num(value)) {
        print_bad_value(opt_name, value);
        exit(1);
    }
    // Parse to int
    int n = toint(value);

    // Check bounds
    if (n < min && min == 1) {
        print_oob(opt_name, ">", 0);
        exit(1);
    } else if (n < min) {
        print_oob(opt_name, ">=", min);
        exit(1);
    } else if (n > max) {
        print_oob(opt_name, "<=", max);
        exit(1);
    }
    // Set flag
    *opt = n;
}

// Parse args
void parse_opt (char **args)
{
    size_t i = 1;

    // Option phase
    while (args[i] && args[i][0] == '-') {
        if (str_is(args[i], "-f") && args[i+1]) {
            set_opt("first ttl", &(g_flags.first_ttl), args[i+1], 1, 255);
        }
        else if (str_is(args[i], "-m") && args[i+1]) {
            set_opt("max ttl", &(g_flags.max_ttl), args[i+1], 1, 255);
        }
        else if (str_is(args[i], "-q") && args[i+1]) {
            set_opt("nprobes", &(g_flags.nqueries), args[i+1], 1, 255);
        }
        else if (str_is(args[i], "-w") && args[i+1]) {
            set_opt("wait time", &(g_flags.waittime), args[i+1], 1, 86400);
        }
        else if (str_is(args[i], "-z") && args[i+1]) {
            set_opt("pause msecs", &(g_flags.pausemsecs), args[i+1], 0, 3600000);
        }
        else {
            print_help();
            exit(1);
        }
        i += 2;
    }
    // Host phase
    if (args[i] && args[i+1] == NULL) {
        size_t l = len(args[i]);

        // Check length
        if (l > 64) {
            print_long_host(args[i]);
            exit(1);
        }

        // Copy host
        memcopy(g_flags.host, args[i], l);
        g_flags.host[l] = 0;
    }
    else {
        print_help();
        exit(1);
    }
    // Edge cases
    if (g_flags.first_ttl > g_flags.max_ttl) {
        print_ttl_issue(g_flags.first_ttl, g_flags.max_ttl);
        exit(1);
    }
    // Finish by setting padding length
    g_flags.padlen = nbrlen(g_flags.max_ttl);
}
