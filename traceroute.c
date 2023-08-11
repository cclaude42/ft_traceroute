#include "traceroute.h"

////////////////////////////////
// Structs
////////////////////////////////

// Init flags with defaults
struct s_flags g_flags = {
    .first_ttl = 1,
    .max_ttl = 64,
    .nqueries = 3,
    .waittime = 5,
    .pausemsecs = 0,
    .padlen = 0,
    .finished = 0,
};

// Init prev
struct sprev prev = {0};


////////////////////////////////
// Network functions
////////////////////////////////

// Get address from ip
void get_addr (char *ip, void *addr)
{
    struct addrinfo *result;

    // Get possibles addresses
    if (getaddrinfo(ip, NULL, NULL, &result)) {
        fprintf(stderr, "ft_traceroute: unknown host %s\n", ip);
        exit(1);
    }
    
    // Iterate through linked list until you find one that works
    for (struct addrinfo *node = result ; node && node->ai_next ; node = node->ai_next) {
        if (((struct sockaddr_in *)node->ai_addr)->sin_addr.s_addr) {
            memcopy(addr, node->ai_addr, sizeof(struct sockaddr_in));
            break ;
        }
    }

    // Free list
    freeaddrinfo(result);
}

// Open socket with specified protocol
int open_socket (int proto)
{
    int sockfd = socket(AF_INET, SOCK_RAW, proto);

    if (sockfd < 0) {
        fprintf(stderr, "ft_traceroute: fatal error: couldn't open socket\n");
        exit(2);
    }
    return sockfd;
}

// Set socket timeout
void set_timeout (int sockfd)
{
    struct timeval tv = {
        .tv_sec = g_flags.waittime,
        .tv_usec = 0
    };

    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
}


////////////////////////////////
// ft_traceroute
////////////////////////////////

// Watch for ICMP reply
void recv_reply (int sockfd)
{
    struct sockaddr_in  from;
    socklen_t           addrlen;
    char                buf[1000];

    while (1) {
        memzero(&buf, sizeof(buf));
        memzero(&from, sizeof(from));
        addrlen = sizeof(from);

        int rcvd = recvfrom(sockfd, &buf, sizeof(buf), 0, (struct sockaddr *)&from, &addrlen);

        // Got timed out
        if (rcvd == -1) {
            print_star();
            prev.sameline = 0;
            return ;
        }

        // Watch for start
        if (buf[ICMP_TYPE] == TTL_EXCEEDED && buf[ICMP_CODE] == TTL_EXCEEDED_TRANSIT) {
            break;
        }

        // Watch for end
        if (buf[ICMP_TYPE] == DESTINATION_UNREACHABLE && buf[ICMP_CODE] == PORT_UNREACHABLE) {
            g_flags.finished = 1;
            break;
        }
    }

    // Print new addresses and timestamps
    int addr_diff = memcomp(&(from.sin_addr), &(prev.ip), sizeof(struct in_addr));
    if (addr_diff) {
        if (prev.sameline)
            print_newline();
        print_address(from.sin_addr);
    }
    print_timestamp(prev.sent, getnow());

    // Setup next print
    prev.ip = from.sin_addr;
    prev.sameline = 1;
}

// Send UDP request
void send_request (int sockfd, struct sockaddr_in addr, int ttl, int port_offset)
{
    // Init packet
    struct trace_packet request = {
        .srcport = byteswapped(SRCPORT_DEFAULT),
        .dstport = byteswapped(DSTPORT_DEFAULT + port_offset),
        .len = byteswapped(sizeof(request)),
        .checksum = CHECKSUM_DEFAULT,
    };

    // Set TTL
    setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));

    // Send packet
    int ret = sendto(sockfd, &request, sizeof(request), 0, (struct sockaddr *)&addr, sizeof(addr));
    prev.sent = getnow();

    if (ret < 0) {
        fprintf(stderr, "ft_traceroute: fatal error: couldn't send packet\n");
        exit(2);
    }
}

// ft_traceroute
void ft_traceroute (int sock_udp, int sock_icmp)
{
    // Get address from host
    struct sockaddr_in addr = {0};
    get_addr(g_flags.host, &addr);

    print_init(addr);
    // Loop through TTLs
    for (int ttl = g_flags.first_ttl ; ttl < g_flags.max_ttl + 1 ; ttl++) {
        // Reset prev and print ttl
        memzero(&prev, sizeof(prev));
        print_newttl(ttl);

        // Loop through probes
        for (int nq = 0 ; nq < g_flags.nqueries ; nq++) {
            send_request(sock_udp, addr, ttl, ttl * g_flags.nqueries + nq);
            recv_reply(sock_icmp);
            waitfor(g_flags.pausemsecs);
        }

        // Exit if reached host
        if (g_flags.finished) {
            printf("\n");
            exit(0);
        }
    }
}

int main (int ac, char **av)
{
    (void)ac;

    // Parse options
    parse_opt(av);

    // Open sockets
    int sock_udp = open_socket(IPPROTO_UDP);
    int sock_icmp = open_socket(IPPROTO_ICMP);
    set_timeout(sock_icmp);

    // Run traceroute
    ft_traceroute(sock_udp, sock_icmp);

    return 0;
}