#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

void ex02_program() {

    printf("I'm...\n");

    pid_t p = fork();

    if (p > 0) {
        printf("the...\n");
        p = fork();
    }
    
    if (p > 0) {
        printf("father!\n");
        p = fork();
    }

    if (p == 0)
        printf("I'll never join you! This process PID: %d\n", getpid());
}
