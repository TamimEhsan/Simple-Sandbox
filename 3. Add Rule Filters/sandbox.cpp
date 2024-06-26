#include <stdio.h>
#include <seccomp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <unistd.h>


int main(int argc, char *argv[]) {
    int syscalls_whitelist[] = {
        SCMP_SYS(access),
        SCMP_SYS(arch_prctl),
        SCMP_SYS(brk),
        SCMP_SYS(clock_gettime),
        SCMP_SYS(close),
        SCMP_SYS(exit_group),
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
        return -1;
    }

    for (int i = 0; i < syscalls_whitelist_length; i++) {
        if (seccomp_rule_add(ctx, SCMP_ACT_ALLOW, syscalls_whitelist[i], 0) != 0) {
            return -1;
        }
    }

     if (seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(open), 1, SCMP_CMP(1, SCMP_CMP_MASKED_EQ, O_WRONLY | O_RDWR, 0)) != 0) {
        return -1;
    }
    if (seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(openat), 1, SCMP_CMP(2, SCMP_CMP_MASKED_EQ, O_WRONLY | O_RDWR, 0)) != 0) {
        return -1;
    }


    // extra rule for execve
    // if (seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(execve), 1, SCMP_A0(SCMP_CMP_EQ, (scmp_datum_t)(_config->exe_path))) != 0) {
    //     return -1;
    // }
    

    if (seccomp_load(ctx) != 0) {
        return -1;
    }
    seccomp_release(ctx);

    
    // Path to the external program
    char *program;
    if( argc == 1 ){
        return 0;
    }
    program = argv[1];
    char *args[] = {program, NULL};
    char *env[] = {NULL};
    
    // Execute the external program
    if (execve(program, args, env) == -1) {
        perror("execve");
        return 1;
    }


    return 0;
}