#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "net.h"
#define MSGPID
#include "../tools/alerts.h"

struct sockaddr_in baddr; // Broadcast address
struct net_msg msg;

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
        PRINT("Sent number of cores");

    while(1);

    shutdown(sock, SHUT_RDWR);
    close(sock);
    return 0;
}
