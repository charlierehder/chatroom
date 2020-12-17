#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

#include "common.h"

size_t get_file_size (int fd) {

    struct stat sb;
    fstat (fd, &sb);
    return (size_t) sb.st_size;

}

ssize_t write_to_fd (int fd, void *buffer, size_t count) {

    ssize_t total_bytes = 0;
    ssize_t bytes_read;

    while (total_bytes < count) {

        bytes_read = write(fd, (char *)buffer + total_bytes, count - total_bytes);

        if (bytes_read > 0) {
            total_bytes += bytes_read;

        } else if (bytes_read == 0) {
            return total_bytes;

        } else if (bytes_read < 0 && errno == EINTR) {
            continue;

        } else {
            return -1;

        }

    }

    return total_bytes;

}

ssize_t read_from_fd (int fd, void *buffer, size_t count) {

    ssize_t total_bytes = 0;
    ssize_t bytes_read;

    while (total_bytes < count) {

        bytes_read = read(fd, (char *)buffer + total_bytes, count - total_bytes);

        if (bytes_read > 0) {
            total_bytes += bytes_read;

        } else if (bytes_read == 0) {
            return total_bytes;

        } else if (bytes_read < 0 && errno == EINTR) {
            return -1;

        }
        
    }

    return total_bytes;

}