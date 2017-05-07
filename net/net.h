#define DEBUG
#define BROADCAST_PORT  10001
#define FUNC(x)         x

struct net_msg {
    int tcp_port;
    unsigned cores, steps;
    long double interval_start,
                interval_end,
                h;
};
