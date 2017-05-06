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


char buffer[BUFSIZ];
struct sockaddr_in sock_in;

int main(int argc, char** argv)
{
    int sock, recv_bytes;
    socklen_t sock_in_len = sizeof(struct sockaddr_in);

    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    sock_in.sin_addr.s_addr = htonl(INADDR_ANY);
    sock_in.sin_port = htons(BROADCAST_PORT);
    sock_in.sin_family = PF_INET;

    int ld1 = 1;
    ERRTEST(setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &ld1, sizeof(ld1)));
    ERRTEST(bind(sock, (struct sockaddr*)&sock_in, sock_in_len));

    ERRTEST(recv_bytes = recvfrom(sock, buffer, BUFSIZ-1, 0, (struct sockaddr *)&sock_in, &sock_in_len));
    buffer[recv_bytes] = '\0';
    PRINT("Received %d bytes: %s", recv_bytes, buffer);

    shutdown(sock, 2);
    close(sock);
}
