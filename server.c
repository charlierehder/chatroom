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
#include "vector.h"

#define MAX_CLIENTS 10
#define NAME_SIZE 16

vector *clients = NULL;

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

    signal(SIGPIPE, SIG_IGN);

    while(1) {

        size_t msg_size = 0;
        read_from_fd(client_fd, &msg_size, 8);

        char msg[msg_size];
        read_from_fd(client_fd, msg, msg_size);
        if (strncmp("!exit", msg, 5) == 0) {
            break;
        }

        size_t n_msg_size = msg_size + 2 + strlen(username);

        for (size_t i = 0; i < vector_size(clients); i++) {

            int write_fd = ((clientinfo *)vector_get(clients, i))->client_fd;

            if (write_fd != client_fd) {

                write_to_fd(write_fd, &n_msg_size, 8);
                write_to_fd(write_fd, username, strlen(username));
                write_to_fd(write_fd, ": ", 2);
                write_to_fd(write_fd, msg, msg_size);

            }
            
        }

    }

    for (size_t i = 0; i < vector_size(clients); i++) {

        clientinfo *current_elem = (clientinfo *)vector_get(clients, i);
        if (current_elem->client_id == client_id) {
            vector_remove(clients, i);
            break;
        }

    }

    free(args);
    pthread_exit(NULL);

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

    clients = vector_create();

    while (1) {

        int client_fd = accept(sock_fd, NULL, NULL);

        if (client_fd == -1) {
            LOG("accept connection failure");
            continue;            
        }

        if (vector_size(clients) == MAX_CLIENTS) {
            LOG("max clients reached")
            continue;
        }

        LOG("new connection established");

        clientinfo *client = (clientinfo *)malloc(sizeof(clientinfo));
        client->client_fd = client_fd;
        client->client_id = client_id;

        size_t msg_size = 0;
        read_from_fd(client_fd, (void *)(&msg_size), 8);
        ssize_t bytes_read = read_from_fd(client_fd, client->username, msg_size);
        vector_push_back(clients, client);

        fprintf(stderr, "%s connected\n", client->username);

        pthread_create(&threads[client_id], NULL, handle_client, (void *)client);
        pthread_detach(threads[client_id]);

        client_id++;

    }
}