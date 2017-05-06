#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "net.h"
#define MSGPID
#define DEBUG
#include "../tools/alerts.h"

int bsock; // Socket for broadcasting

void* broadcast(void* args) {
    char buffer[BUFSIZ] = "Hello, broad world!";
    struct sockaddr_in addr;

    int sent_bytes, buffer_len;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    
    if (!(bsock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)))
        ERROR("Failed to create a socket");

    memset(&addr, 0, addr_len);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(0);
    addr.sin_family = PF_INET;

    ERRTEST(bind(bsock, (struct sockaddr*)&addr, addr_len));
    int ld1 = 1;
    ERRTEST(setsockopt(bsock, SOL_SOCKET, SO_BROADCAST, &ld1, sizeof(ld1)));
    addr.sin_addr.s_addr = htonl(-1);
    addr.sin_port = htons(BROADCAST_PORT);

    buffer_len = strlen(buffer);
    while (1) {
        ERRTEST(sent_bytes = sendto(bsock, buffer, buffer_len, 0, (struct sockaddr*)&addr, addr_len));
        PRINT("Sent %d bytes", sent_bytes);
        sleep(1);
    }
}


int main(int argc, char** argv)
{
    int clients_max;
    if (argc < 2 || !sscanf(argv[1], "%d", &clients_max))
        ERROR("Usage: %s [CLIENTS]", argv[0]);

    int master, *clients = calloc(clients_max, sizeof(int));
    if (!(master = socket(PF_INET, SOCK_STREAM, 0)))
        ERROR("Failed to create a socket");

    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    memset(&addr, 0, addr_len);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = TCP_PORT;

    ERRTEST(bind(master, (struct sockaddr*)&addr, addr_len));
    ERRTEST(listen(master, clients_max));

    // Start the broadcast
    pthread_t bthread;
    if (pthread_create(&bthread, NULL, &broadcast, NULL))
        ERROR("Failed to create the broadcasting thread");

    fd_set fds;
    int clients_count = 0, maxsd, recv_bytes;
    char buffer[BUFSIZ];

    while (clients_count < clients_max) {
        FD_ZERO(&fds);
        FD_SET(master, &fds);
        maxsd = master;
        for (int i = 0; i < clients_max; ++i) {
            if (clients[i]) {
                FD_SET(clients[i], &fds);
                if (clients[i] > maxsd)
                    maxsd = clients[i];
            }
        }

        if ((select(maxsd+1, &fds, NULL, NULL, NULL) == -1) && (errno != EINTR))
            ERROR("Select failed");

        if (FD_ISSET(master, &fds)) {
            int new;
            ERRTEST(new = accept(master, (struct sockaddr*)&addr, &addr_len));
            for (int i = 0; i < clients_max; ++i) {
                if (!clients[i])
                    clients[i] = new;
                PRINT("New connection (%d) [%s:%d]",
                    clients_count, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
            }
        }

        for (int i = 0; i < clients_max; ++i)
            if (FD_ISSET(clients[i], &fds)) {
                ERRTEST(recv_bytes = read(clients[i], buffer, BUFSIZ));
                if (!recv_bytes) {
                    // Connection closed
                    close(clients[i]);
                    clients[i] = 0;
                    PRINT("Connection (%d) closed", i);
                }
            }
    }

    // Finish the broadcast
    pthread_cancel(bthread);
    pthread_join(bthread, NULL);
    shutdown(bsock, 2);
    close(bsock);

    return 0;
}
