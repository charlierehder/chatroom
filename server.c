#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "common.h"

#define MAX_CLIENTS 10
#define NAME_SIZE 16

int clients[MAX_CLIENTS];

static int available_clients = 0;

typedef struct {
    int client_fd;
    int client_id;
    char username[NAME_SIZE];
} clientinfo;

void *handle_client(void *args) {

    clientinfo *client = (clientinfo *)args;
    int client_fd = client->client_fd;
    int client_id = client->client_id;
    char *username = client->username;

    while(1) {

        size_t msg_size = 0;
        read_from_fd(client_fd, &msg_size, 8);

        char msg[msg_size];
        read_from_fd(client_fd, msg, msg_size);

        size_t n_msg_size = msg_size + 2 + strlen(username);

        for (size_t i = 0; i < available_clients; i++) {

            write_to_fd(clients[i], &n_msg_size, 8);

            write_to_fd(clients[i], username, strlen(username));
            write_to_fd(clients[i], ": ", 2);
            write_to_fd(clients[i], msg, msg_size);
            
        }

    }

    return NULL;

}

int main(int argc, char *argv[]) {
    
    char *port = argv[1];

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct addrinfo hints, *res;
    
    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int s = getaddrinfo(NULL, port, &hints, &res);
    if (s != 0) {
        LOG("getaddrinfo: %s\n", gai_strerror(s));
        return 0;
    }

    int bind_res = bind(sock_fd, res->ai_addr, res->ai_addrlen);
    if (bind_res != 0) {
        LOG("bind failure");       
        return 0;
    }

    int listen_res = listen(sock_fd, 10);
    if (listen_res != 0) {
        LOG("listen failure");
        return 0;
    }

    int client_id = 0;
    pthread_t threads[MAX_CLIENTS];

    while (1) {

        int client_fd = accept(sock_fd, NULL, NULL);

        if (client_fd == -1) {
            LOG("accept connection failure");
            continue;            
        }

        if (client_id == MAX_CLIENTS) {
            LOG("max clients reached")
            continue;
        }

        LOG("new connection established");
        clients[client_id] = client_fd;
        available_clients++;

        clientinfo *client = (clientinfo *)malloc(sizeof(clientinfo));
        client->client_fd = client_fd;
        client->client_id = client_id;

        size_t msg_size = 0;
        read_from_fd(client_fd, (void *)(&msg_size), 8);
        ssize_t bytes_read = read_from_fd(client_fd, client->username, msg_size);

        fprintf(stderr, "%s connected\n", client->username);

        pthread_create(&threads[client_id], NULL, handle_client, (void *)client);

        client_id++;

    }
}