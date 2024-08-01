#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "task.h"

volatile sig_atomic_t flag = 0;

void handle_USR1(int signo, siginfo_t *sinfo, void *context) {
	write(STDOUT_FILENO, "Child process received signal from parent\n", 42);
	flag = 1;    
}

int main() {
    pid_t p = fork();

	/* Caso aconteça uma falha na criação do processo filho */
	if (p == -1) {
		perror("Fork falhou");
		exit(EXIT_FAILURE);
	}
    
	if (p == 0) {
		struct sigaction act;
		
		memset(&act, 0, sizeof(struct sigaction));
		sigemptyset(&act.sa_mask);
		act.sa_sigaction = handle_USR1;
		act.sa_flags = SA_SIGINFO;

		sigaction(SIGUSR1, &act, NULL);

		task_b();
		
		while (!flag) {
			pause();
		}

		task_c();

		exit(0);
    } else if (p > 0) {
		task_a();
		
		kill(p, SIGUSR1);
        
		wait(NULL);
        printf("Job is complete!\n");
    }

    return 0;
}