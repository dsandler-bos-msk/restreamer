/**
 * Restremer - is a basic UDP repeater. It starts a UDP server accepting any UDP packets on PORT_IN, and then,
 * retransmits them to UDP servers starting with port PORT_OUT_START, and incrementing the port number PORT_OUT_COUNT
 * times.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER 4096
uint8_t buffer[BUFFER];

void runner(uint64_t port_in, struct sockaddr_in *client_addrs, size_t clients_count);

void print_usage();

int main(int argc, char *argv[]) {
    in_port_t port_in;
    // first param is executable, second is in_port, the rest is pairs of IP and port num
    size_t clients_count = (argc - 2) / 2;
    struct sockaddr_in client_addrs[clients_count];
    memset(&client_addrs, 0, sizeof(struct sockaddr_in) * clients_count);

    if (argc >= 4 && argc % 2 == 0) {
        port_in = strtol(argv[1], NULL, 10);

        for (size_t i = 2; i < argc; i += 2) {
            client_addrs[i / 2 - 1].sin_family = AF_INET;
            client_addrs[i / 2 - 1].sin_port = htons(strtol(argv[i + 1], NULL, 10));
            if (inet_pton(AF_INET, argv[i], &(client_addrs[i / 2 - 1].sin_addr)) <= 0) {
                perror("Couldn't parse an IPv4 address!");
                print_usage();
                exit(EXIT_FAILURE);
            }
        }
    } else {
        print_usage();
        exit(EXIT_FAILURE);
    }

    printf("PORT_IN: %hu, DESTINATIONS:\n", port_in);
    char ipv4_addr[INET_ADDRSTRLEN];
    in_port_t sin_port;
    for (size_t i = 0; i < clients_count; i++) {
        inet_ntop(AF_INET, &(client_addrs[i].sin_addr), ipv4_addr, INET_ADDRSTRLEN);
        sin_port = (client_addrs[i].sin_port & 0xff00) >> 8 | (client_addrs[i].sin_port & 0xff) << 8;
        printf("%s:%i\n", ipv4_addr, sin_port );
    }

    runner(port_in, client_addrs, clients_count);
}

void print_usage() {
    printf("restreamer PORT_IN <IPV4_OUT_1 PORT_OUT_1, IPV4_OUT_2 PORT_OUT_2, ...>\n");
}

void runner(uint64_t port_in, struct sockaddr_in *client_addrs, size_t clients_count) {
    int sock_fd;
    int sender_sock_fd;

    struct sockaddr_in acceptor_addr;
    struct sockaddr_in source_addr;
    memset(&acceptor_addr, 0, sizeof(acceptor_addr));
    memset(&source_addr, 0, sizeof(source_addr));

    socklen_t addr_len = sizeof(source_addr);
    ssize_t received_count;
    ssize_t sent_count;

    acceptor_addr.sin_family = AF_INET;
    acceptor_addr.sin_addr.s_addr = INADDR_ANY;
    acceptor_addr.sin_port = htons(port_in);

    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    if (bind(sock_fd, (const struct sockaddr *) &acceptor_addr, sizeof(acceptor_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if ((sender_sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while (true) {
        received_count = recvfrom(sock_fd, buffer, BUFFER,
                                  MSG_WAITALL, (struct sockaddr *) &source_addr,
                                  &addr_len);
#ifdef DEBUG
        printf("udp count: %zu\n", received_count);
#endif

        for (size_t i = 0; i < clients_count; i++) {
            sent_count = sendto(sender_sock_fd, buffer, received_count,
                                0, (const struct sockaddr *) (&client_addrs[i]),
                                sizeof(client_addrs[i]));
#ifdef DEBUG
            printf("sent count to client %zu count: %zu\n", i, sent_count);
#endif
        }
    }
#pragma clang diagnostic pop
}
