
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

int main(int argc, char *argv[]){
    
    char *program;
    if( argc > 1 ){
        program = argv[1];
    }
    char *args[] = {program, NULL};
    char *env[] = {NULL};

    if( execve(program, args,env) == -1 ){
        printf("Error: %d\n", errno);
        return 1;
    }
    return 0;
}