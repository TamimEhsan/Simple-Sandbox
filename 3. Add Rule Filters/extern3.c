#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>


int main(){
    int fd = open("input.txt", O_RDONLY);
     // Read data from the file
    char buffer[1024];
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer));
    if (bytes_read == -1) {
        perror("read");
        close(fd);
        return 1;
    }

    // Close the file
    close(fd);

    // Print the data read from the file
    printf("Data read from file:\n%.*s\n", (int)bytes_read, buffer);

    return 0;

}