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

#define BUF_SIZE 1024

void *listen_to_server(void *args) {
    
    int socket_fd = *((int *)args);

    while (1) {

        size_t msg_size = 0;
        ssize_t bytes_read = read_from_fd(socket_fd, &msg_size, 8);

        char buffer[msg_size];
        read_from_fd(socket_fd, buffer, msg_size);

        write_to_fd(1, buffer, msg_size);

    }

}

void *listen_to_stdin(void *args) {

    int socket_fd = *((int *)args);

    while (1) {

        char *buffer;
        size_t size = 0;
        ssize_t bytes_read = getline(&buffer, &size, stdin);
        write_to_fd(socket_fd, &bytes_read, 8);
        write_to_fd(socket_fd, buffer, bytes_read);

    }
    
}


int main(int argc, char *argv[]){

    char *host = argv[1];
    char *port = argv[2];

    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int s = getaddrinfo(host, port, &hints, &res);
    if (s != 0) {
        LOG("getaddrinfo: %s\n", gai_strerror(s));
        return 0;
    }

    int socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (socket_fd == -1) {
        LOG("socket failure");
        return 0;
    }

    int connect_res = connect(socket_fd, res->ai_addr, res->ai_addrlen);
    if (connect_res == -1) {
        LOG("connection failure");
        return 0;
    }

    LOG("connection success");
    LOG("Please enter username")

    char *username = (char *)malloc(16 * sizeof(char));
    size_t size = 16;
    ssize_t bytes_read = getline(&username, &size, stdin);
    username[bytes_read - 1] = '\0'; 

    write_to_fd(socket_fd, (void *)(&bytes_read), 8);
    write_to_fd(socket_fd, username, bytes_read);

    pthread_t threads[2];
    pthread_create(&threads[0], NULL, listen_to_server, &socket_fd);
    pthread_create(&threads[1], NULL, listen_to_stdin, &socket_fd);

    while(1) {}

}