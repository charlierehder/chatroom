#include <stdlib.h>
#include <stdio.h>

#define LOG(args...) fprintf(stderr, args); fprintf(stderr, "\n");

/*
* Assumes 'fd' is valid and open. Uses stat to retrieve and return  
* the size of the file pointed to by 'fd' as a size_t.
*/
size_t get_file_size(int fd);

/*
* Writes 'count' bytes from 'buffer' to 'fd'. Assumes 'fd' is open
* and valid. Also assumes 'buffer' as at least 'count' bytes to write
*
* Returns the number of bytes written on success or -1 if an error 
* occurs at some point during runtime. Ignores EINTR errors.
*/
ssize_t write_to_fd (int fd, void *buffer, size_t count);

/*
* Reads 'count' bytes to 'buffer' from 'fd'. Assumes 'fd' is open 
* and valid. Also assumes 'buffer' has at least 'count' bytes to write to.
*
* Returns the number of bytes read on success or -1 if an error occurs
* at some point during runtime. Ignores EINITR errors.
*/
ssize_t read_from_fd (int fd, void *buffer, size_t count);