#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "./net.h"
#define MSGPID
#include "../tools/alerts.h"

struct sockaddr_in baddr;
struct net_msg msg;
long double h, h2, S;

void recv_broadcast() {
    int bsock, recv_bytes;
    socklen_t baddr_len = sizeof(struct sockaddr_in);

    ERRTEST(bsock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP));
    baddr.sin_addr.s_addr = htonl(INADDR_ANY);
    baddr.sin_port = htons(BROADCAST_PORT);
    baddr.sin_family = AF_INET;

    int ld1 = 1;
    ERRTEST(setsockopt(bsock, SOL_SOCKET, SO_REUSEPORT, &ld1, sizeof(ld1)));
    ERRTEST(bind(bsock, (struct sockaddr*)&baddr, baddr_len));

    ERRTEST(recv_bytes = recvfrom(bsock, &msg, sizeof(struct net_msg),
        0, (struct sockaddr *)&baddr, &baddr_len));
    if (recv_bytes != sizeof(struct net_msg))
        ERROR("Net message receiving failed");
    PRINT("Received %d bytes; server port %d", recv_bytes, msg.tcp_port);
    baddr.sin_port = msg.tcp_port;

    shutdown(bsock, SHUT_RDWR);
    close(bsock);
}


long double* data;
pthread_t* threads;

void* simpson(void* args) {
    long double lh = h,
                lh2 = h2,
                a = *(long double*)args,
                b = a + lh2,
                c = a + lh,
                func_a = FUNC(a),
                func_c,
                sum = 0;
    unsigned steps = msg.steps;

    for (unsigned i = 0; i < steps; ++i) {
        func_c = FUNC(c);
        sum += func_a + 4 * FUNC(b) + func_c;
        func_a = func_c;
        a = c;
        b += lh;
        c += lh;
    }

    *(long double*)args = sum * h/6;
    PRINT("%Lf", *(long double*)args);
    pthread_exit(args);
}


int main()
{
    recv_broadcast();

    int sock;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);

    ERRTEST(sock = socket(PF_INET, SOCK_STREAM, 0));
    memset(&addr, 0, addr_len);
    addr.sin_addr.s_addr = baddr.sin_addr.s_addr;
    addr.sin_port = baddr.sin_port;
    addr.sin_family = AF_INET;

    int ld1 = 1;
    ERRTEST(setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &ld1, sizeof(ld1)));
    ERRTEST(connect(sock, (struct sockaddr*)&addr, addr_len));
    getsockname(sock, (struct sockaddr*)&baddr, &addr_len);
    PRINT("Connected (port %d)", ntohs(baddr.sin_port));

    msg.cores = 1;
    int wrote_bytes;
    ERRTEST(wrote_bytes = write(sock, &msg, sizeof(struct net_msg)));
    if (wrote_bytes != sizeof(struct net_msg))
        ERROR("Net message sending failed")
    else
        PRINT("Ready for calculation");

    int bytes;
    ERRTEST(bytes = read(sock, &msg, sizeof(struct net_msg)));
    if (!bytes)
        ERROR("Connection closed")
    else if (bytes != sizeof(struct net_msg))
        ERROR("Net message receiving failed");

    if (!(threads = malloc(msg.cores * sizeof(pthread_t))) ||
        !(data = malloc(msg.cores * sizeof(long double))))
        ERROR("Memory allocation failed");

    h = msg.h;
    h2 = h/2;
    long double interval_start,
                interval_len = (msg.interval_end - msg.interval_start)/msg.cores;
    unsigned i;

    for (i = 0, interval_start = msg.interval_start;
         i < msg.cores;
         ++i, interval_start += interval_len) {
        data[i] = interval_start;
        if (pthread_create(&threads[i], NULL, &simpson, &data[i]))
            ERROR("Thread creation failed");
    }

    long double S = 0, *r;
    for (i = 0; i < msg.cores; ++i) {
        if (pthread_join(threads[i], (void**) &r))
            ERROR("Thread joining failed");
        S += *r;
    }

    PRINT("Result = %.12Lf\n", S);

    shutdown(sock, SHUT_RDWR);
    close(sock);
    return 0;
}
