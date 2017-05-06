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


char buffer[BUFSIZ] = "Hello, broad world!";
struct sockaddr_in sock_in;

int main(int argc, char** argv)
{
    int sock, sent_bytes, buffer_len;
    socklen_t sock_in_len = sizeof(struct sockaddr_in);
    
    sock = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    sock_in.sin_addr.s_addr = htonl(INADDR_ANY);
    sock_in.sin_port = htons(0);
    sock_in.sin_family = PF_INET;

    ERRTEST(bind(sock, (struct sockaddr*)&sock_in, sock_in_len));
    int ld1 = 1;
    ERRTEST(setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &ld1, sizeof(ld1)));
    sock_in.sin_addr.s_addr = htonl(-1);
    sock_in.sin_port = htons(BROADCAST_PORT);

    buffer_len = strlen(buffer);
    ERRTEST(sent_bytes = sendto(sock, buffer, buffer_len, 0, (struct sockaddr *)&sock_in, sock_in_len));
    PRINT("Sent %d bytes", sent_bytes);

    shutdown(sock, 2);
    close(sock);
}
