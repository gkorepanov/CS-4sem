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
#include "../tools/alerts.h"

struct sockaddr_in baddr;
struct net_msg msg;
int sock, bytes;
struct sockaddr_in addr;
socklen_t addr_len = sizeof(struct sockaddr_in);
long double h, h2;
long double* data;
pthread_t* threads;

void recv_broadcast();
void wait_for_job();
void* simpson(void* args);

int N;

void arg_process(int argc, char** argv) {
    if (argc != 2)
        ERROR("Usage: %s [NUMBER OF THREADS]",
            argv[0]);

    if (!sscanf(argv[1], "%d", &N))
        ERROR("Invalid argument (type <int>)");
}


int main(int argc, char** argv) {
    arg_process(argc, argv);

    recv_broadcast();
    wait_for_job();

    if (!(threads = malloc(msg.cores * sizeof(pthread_t))) ||
        !(data = malloc(msg.cores * sizeof(long double))))
        ERROR("Memory allocation failed");

    h = msg.h;
    h2 = h/2;
    long double interval_start = msg.interval_start,
                interval_len = (msg.interval_end - msg.interval_start)/msg.cores;

    PRINT("Calculation started");
    PRINT("Int [%.6Lf:%.6Lf] (%u steps)",
        msg.interval_start, msg.interval_end, msg.steps);

    for (unsigned i = 0; i < msg.cores; ++i, interval_start += interval_len) {
        data[i] = interval_start;
        if (pthread_create(&threads[i], NULL, &simpson, &data[i]))
            ERROR("Thread creation failed");
    }

    long double S = 0, *r;
    for (unsigned i = 0; i < msg.cores; ++i) {
        if (pthread_join(threads[i], (void**) &r))
            ERROR("Thread joining failed");
        S += *r;
    }
    PRINT("Result = %.6Lf", S);

    msg.h = S;
    ERRTEST(bytes = write(sock, &msg, sizeof(struct net_msg)));
        if (bytes != sizeof(struct net_msg))
            ERROR("Net message sending failed");

    PRINT("Done.")
    shutdown(sock, SHUT_RDWR);
    close(sock);
    free(threads);
    free(data);
    exit(EXIT_SUCCESS);
}


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

    PRINT("Waiting for the server...");

    ERRTEST(recv_bytes = recvfrom(bsock, &msg, sizeof(struct net_msg),
        0, (struct sockaddr *)&baddr, &baddr_len));
    if (recv_bytes != sizeof(struct net_msg))
        ERROR("Net message receiving failed");
    PRINT("Received %d bytes; server port %d", recv_bytes, msg.tcp_port);
    baddr.sin_port = msg.tcp_port;

    shutdown(bsock, SHUT_RDWR);
    close(bsock);
}


void wait_for_job() {
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

    //msg.cores = sysconf(_SC_NPROCESSORS_ONLN);

    msg.cores = N;
    
    ERRTEST(bytes = write(sock, &msg, sizeof(struct net_msg)));
    if (bytes != sizeof(struct net_msg))
        ERROR("Net message sending failed")
    else
        PRINT("Waiting for job...");

    ERRTEST(bytes = read(sock, &msg, sizeof(struct net_msg)));
    if (!bytes)
        ERROR("Connection closed")
    else if (bytes != sizeof(struct net_msg))
        ERROR("Net message receiving failed");
}


void* simpson(void* args) {
    long double lh = h,
                lh2 = h2,
                a = *(long double*)args,
                b = a + lh2,
                c = a + lh,
                func_a = FUNC(a),
                func_c,
                sum = 0;
    unsigned steps = msg.steps/msg.cores;

    PRINT("Thread int [%.6Lf:%.6Lf] (%u steps)", a, a + h*steps, steps);
    for (unsigned i = 0; i < steps; ++i) {
        func_c = FUNC(c);
        sum += func_a + 4 * FUNC(b) + func_c;
        func_a = func_c;
        a = c;
        b += lh;
        c += lh;
    }

    *(long double*)args = sum * h/6;
    pthread_exit(args);
}
