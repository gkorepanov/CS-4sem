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
#include "../tools/alerts.h"

int bsock;   // Socket for broadcasting
int *cores;  // Clients' numbers of cores

void* broadcast(void* args) {
    struct sockaddr_in addr;

    int sent_bytes;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    
    ERRTEST(bsock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP));
    memset(&addr, 0, addr_len);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(0);
    addr.sin_family = AF_INET;

    ERRTEST(bind(bsock, (struct sockaddr*)&addr, addr_len));
    int ld1 = 1;
    ERRTEST(setsockopt(bsock, SOL_SOCKET, SO_BROADCAST, &ld1, sizeof(ld1)));
    addr.sin_addr.s_addr = htonl(-1);
    addr.sin_port = htons(BROADCAST_PORT);

    while (1) {
        ERRTEST(sent_bytes = sendto(bsock, args, sizeof(struct net_msg),
            0, (struct sockaddr*)&addr, addr_len));
        if (sent_bytes != sizeof(struct net_msg))
            ERROR("Net message sending failed")
        PRINT("Sent %d bytes", sent_bytes);
        sleep(1);
    }

    args = NULL;
}


int main(int argc, char** argv)
{
    int clients_max;
    if (argc < 2 || !sscanf(argv[1], "%d", &clients_max))
        ERROR("Usage: %s [CLIENTS]", argv[0]);

    int master, *clients, *cores;
    if (!(clients = calloc(clients_max, sizeof(int))) ||
        !(cores   = calloc(clients_max, sizeof(int))))
        ERROR("Memory allocation failed");

    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);

    ERRTEST(master = socket(PF_INET, SOCK_STREAM, 0));
    memset(&addr, 0, addr_len);
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(0);
    addr.sin_family = AF_INET;

    ERRTEST(bind(master, (struct sockaddr*)&addr, addr_len));
    ERRTEST(listen(master, clients_max));
    ERRTEST(getsockname(master, (struct sockaddr*)&addr, &addr_len));

    struct net_msg broadcast_msg;
    memset(&broadcast_msg, 0, sizeof(struct net_msg));
    broadcast_msg.tcp_port = addr.sin_port;

    // Start the broadcast
    pthread_t bthread;
    if (pthread_create(&bthread, NULL, &broadcast, &broadcast_msg))
        ERROR("Failed to create the broadcasting thread");

    fd_set fds;
    int clients_count = 0, cores_info = 0, maxsd, recv_bytes;
    struct net_msg recv_msg;

    PRINT("Waiting for clients (port %d)", broadcast_msg.tcp_port);
    while (cores_info < clients_max) {
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

        if (clients_count == clients_max) {
            FD_CLR(master, &fds);
            shutdown(master, SHUT_RDWR);
            close(master);
        }

        if ((select(maxsd+1, &fds, NULL, NULL, NULL) == -1) && (errno != EINTR))
            ERRORV("Select failed");

        if (FD_ISSET(master, &fds)) {
            PRINT("Event @%d", master);
            int new;
            ERRTEST(new = accept(master, (struct sockaddr*)&addr, &addr_len));
            PRINT("New connection (%d) [%s:%d]",
                    clients_count, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
            for (int i = 0; i < clients_max; ++i)
                if (!clients[i]) {
                    clients[i] = new;
                    break;
                }
            clients_count += 1;
        }

        for (int i = 0; i < clients_max; ++i)
            if (FD_ISSET(clients[i], &fds)) {
                PRINT("Event @%d", clients[i]);
                ERRTEST(recv_bytes = read(clients[i], &recv_msg, sizeof(struct net_msg)));
                if (!recv_bytes) {
                    // Connection closed
                    close(clients[i]);
                    clients[i] = 0;
                    cores[i] = 0;
                    PRINT("Connection (%d) closed", i);
                    clients_count -= 1;
                    cores_info -= 1;
                }
                else if (recv_bytes != sizeof(struct net_msg))
                    ERROR("Net message receiving failed")
                else {
                    cores[i] = recv_msg.cores;
                    cores_info += 1;
                    PRINT("Client %d has %d core(s)", i, cores[i]);
                }
            }
    }

    PRINT("Ready to calculate");

    // Finish the broadcast
    pthread_cancel(bthread);
    pthread_join(bthread, NULL);
    shutdown(bsock, SHUT_RDWR);
    close(bsock);

    free(clients);
    free(cores);
    return 0;
}
