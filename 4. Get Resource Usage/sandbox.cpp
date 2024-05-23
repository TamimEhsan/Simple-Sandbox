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

void get_memory_usage(pid_t pid) {
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/statm", pid);

    FILE *file = fopen(path, "r");
    if (file == NULL) {
        perror("fopen");
        return;
    }

    long size, resident, share, text, lib, data, dt;
    if (fscanf(file, "%ld %ld %ld %ld %ld %ld %ld", &size, &resident, &share, &text, &lib, &data, &dt) != 7) {
        perror("fscanf");
        fclose(file);
        return;
    }

    fclose(file);

    long page_size = sysconf(_SC_PAGESIZE);

    printf("Memory usage of process %d:\n", pid);
    printf("Total program size: %ld KB\n", size * page_size / 1024);
    printf("Resident set size: %ld KB\n", resident * page_size / 1024);
    printf("Shared pages: %ld KB\n", share * page_size / 1024);
    printf("Text (code): %ld KB\n", text * page_size / 1024);
    printf("Data/Stack: %ld KB\n", data * page_size / 1024);
}

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
        return 0;
    }
    if( child_pid == 0 ){
        // child
        apply_seccomp_filter();
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
    }else{
        // parent or monitor
        wait(NULL);
        struct rusage usage;
        int status = getrusage(RUSAGE_CHILDREN,&usage);
       
        printf("Memory usage of the program is %ld kb\n",usage.ru_maxrss);


    }


    return 0;
}