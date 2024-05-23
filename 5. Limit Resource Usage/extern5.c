#include <stdio.h>
#include <stdlib.h>

int main() {
    int i = 0;
    char *ptr = NULL;
    
    while (i < 50) {
        ptr = (char*)malloc(1048576); // 1MB allocated
        if (ptr == NULL) {
            printf("Allocation fails at %d MB\n", i);
            return 0;
        }
        printf("Allocated %d MB\n", i + 1);
        i++;
    }
    
    printf("Finished allocation\n");
    
    return 0;
}

// 1336 (350) 1700 (200) 1900