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

#define BUF_SIZE 1024

pthread_mutex_t m =  PTHREAD_MUTEX_INITIALIZER;
int exit_requested = 0; // exit condition

void *listenToServer(void *args) {
    
    int socket_fd = *((int *)args);

    while (1) {

        pthread_mutex_lock(&m);
        if (exit_requested) { pthread_mutex_unlock(&m); break; }
        pthread_mutex_unlock(&m);

        size_t msg_size = 0;
        ssize_t bytes_read = read_from_fd(socket_fd, &msg_size, 8);

        char msg[msg_size];
        read_from_fd(socket_fd, msg, msg_size);
        write_to_fd(1, msg, msg_size);

    }

    pthread_exit(NULL);

}

void *listenToStdin(void *args) {

    int socket_fd = *((int *)args);

    while (1) {

        pthread_mutex_lock(&m);
        if (exit_requested) { pthread_mutex_unlock(&m); break; }
        pthread_mutex_unlock(&m);

        char *msg;
        size_t msg_size = 0;
        ssize_t bytes_read = getline(&msg, &msg_size, stdin);
        if (strncmp("!exit", msg, 5) == 0) {
            LOG("Exiting...");
            pthread_mutex_lock(&m);
            exit_requested = 1;
            pthread_mutex_unlock(&m);
            shutdown(socket_fd, SHUT_RDWR);
            close(socket_fd);
        }
        write_to_fd(socket_fd, &bytes_read, 8);
        write_to_fd(socket_fd, msg, bytes_read);

    }

    pthread_exit(NULL);
    
}


int main(int argc, char *argv[]){

    char *host = argv[1];
    char *port = argv[2];

    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(struct addrinfo));

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
    free(username);

    pthread_t threads[2];
    pthread_create(&threads[0], NULL, listenToServer, &socket_fd);
    pthread_create(&threads[1], NULL, listenToStdin, &socket_fd);

    for (size_t i = 0; i < 2; i++) {
        void *retval = NULL;
        pthread_join(threads[i], retval);
    }

    return 0;
}