#define BROADCAST_PORT  10001
#define TCP_PORT        10002

struct net_msg {
	int tcp_port;
	unsigned cores, steps;
	long double a, b, h;
};
