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
#define DEBUG
#include "../tools/alerts.h"

struct sockaddr_in baddr; // Broadcast address

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

    char buffer[BUFSIZ];
    ERRTEST(recv_bytes = recvfrom(bsock, buffer, BUFSIZ-1, 0, (struct sockaddr *)&baddr, &baddr_len));
    buffer[recv_bytes] = '\0';
    PRINT("Received %d bytes: %s", recv_bytes, buffer);

    shutdown(bsock, 2);
    close(bsock);
}


int main(int argc, char** argv)
{
    recv_broadcast();

    int sock;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);

    ERRTEST(sock = socket(PF_INET, SOCK_STREAM, 0));
    memset(&addr, 0, addr_len);
    addr.sin_addr.s_addr = baddr.sin_addr.s_addr;
    addr.sin_port = htons(TCP_PORT);
    addr.sin_family = AF_INET;

    int ld1 = 1;
    ERRTEST(setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &ld1, sizeof(ld1)));
    ERRTEST(connect(sock, (struct sockaddr*)&addr, addr_len));

    sleep(3);

    shutdown(sock, SHUT_RDWR);
    close(sock);
    return 0;
}
