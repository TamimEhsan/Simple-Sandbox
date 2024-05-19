#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    // Open the file for writing
    int fd = open("input.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    // Data to write to the file
    const char *message = "Hello, file! This is some text written using the open and write system calls.\n";

    // Write data to the file
    ssize_t bytes_written = write(fd, message, strlen(message));
    if (bytes_written == -1) {
        perror("write");
        close(fd);
        return 1;
    }

    // Close the file
    close(fd);

    printf("Data written to file successfully.\n");

    return 0;
}
