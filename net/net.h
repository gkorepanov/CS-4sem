#define DEBUG
#define BROADCAST_PORT  31415
#define FUNC(x)         x*x/(1/x+x-2+x*x)*x
#define SPLIT           2000000000

struct net_msg {
    int tcp_port;
    unsigned cores, steps;
    long double interval_start,
                interval_end,
                h;
};
