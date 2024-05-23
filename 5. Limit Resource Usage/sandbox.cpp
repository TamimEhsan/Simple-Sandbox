#include <stdio.h>
#include <seccomp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <unistd.h>
#include<sys/wait.h>
#include <sys/resource.h>
#include <sys/time.h>


void apply_seccomp_filter() {
    int syscalls_whitelist[] = {
        SCMP_SYS(access),
        SCMP_SYS(arch_prctl),
        SCMP_SYS(brk),
        SCMP_SYS(clock_gettime),
        SCMP_SYS(close),
        SCMP_SYS(exit),
        SCMP_SYS(faccessat),
        SCMP_SYS(fstat),
        SCMP_SYS(futex),
        SCMP_SYS(getrandom),
        SCMP_SYS(lseek),
        SCMP_SYS(mmap),
        SCMP_SYS(mprotect),
        SCMP_SYS(munmap),
        SCMP_SYS(newfstatat),
        SCMP_SYS(pread64),
        SCMP_SYS(prlimit64),
        SCMP_SYS(read),
        SCMP_SYS(readlink),
        SCMP_SYS(readv),
        SCMP_SYS(rseq),
        SCMP_SYS(set_robust_list),
        SCMP_SYS(set_tid_address),
        SCMP_SYS(write),
        SCMP_SYS(writev),
        SCMP_SYS(execve)
    };

    int syscalls_whitelist_length = sizeof(syscalls_whitelist) / sizeof(int);
    scmp_filter_ctx ctx = NULL;

    // load seccomp rules
    ctx = seccomp_init(SCMP_ACT_KILL);
    if (!ctx) {
        return;
    }

    for (int i = 0; i < syscalls_whitelist_length; i++) {
        if (seccomp_rule_add(ctx, SCMP_ACT_ALLOW, syscalls_whitelist[i], 0) != 0) {
            return;
        }
    }

     if (seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(open), 1, SCMP_CMP(1, SCMP_CMP_MASKED_EQ, O_WRONLY | O_RDWR, 0)) != 0) {
        return;
    }
    if (seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(openat), 1, SCMP_CMP(2, SCMP_CMP_MASKED_EQ, O_WRONLY | O_RDWR, 0)) != 0) {
        return;
    }


    // extra rule for execve
    // if (seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(execve), 1, SCMP_A0(SCMP_CMP_EQ, (scmp_datum_t)(_config->exe_path))) != 0) {
    //     return -1;
    // }
    

    if (seccomp_load(ctx) != 0) {
        return;
    }
    seccomp_release(ctx);


}

int main(int argc, char *argv[]) {

    int child_pid = fork();
    if( child_pid == -1 ){
        // failed to fork
        return 0;
    }
    if( child_pid == 0 ){
        // child process

        // apply filter
        apply_seccomp_filter();

        // get the program to execute
        char *program;
        if( argc == 1 ){
            return 0;
        }
        program = argv[1];
        char *args[] = {program, NULL};
        char *env[] = {NULL};
        
        
        // set memory limit to about 10MB
        struct rlimit limits;
        limits.rlim_cur = 1048576*10+10; // set data segment limit to 10MB
        limits.rlim_max = 1048576*10+10; // make sure the child can't increase it again
        setrlimit(RLIMIT_DATA, &limits);

        // Execute the external program
        if (execve(program, args, env) == -1) {
            perror("execve");
            return 1;
        }
    }else{
        // parent or monitor
        int process_exit_status;

        // wait for the child to finish and get exit status
        wait(&process_exit_status);

        // get the signal status
        int signal = WTERMSIG(process_exit_status);
        struct rusage usage;
        int status = getrusage(RUSAGE_CHILDREN,&usage);
       
        printf("Memory usage of the program is %ld kb\n",usage.ru_maxrss);
        printf("Exit status: %d\n", WEXITSTATUS(process_exit_status));
        if( signal == SIGSEGV ) 
            printf("Memory limit exceeded\n");
        printf("Termination signal: %d\n", signal == SIGSEGV);
    }

    return 0;
}