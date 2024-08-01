#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void handle_USR1(int signo, siginfo_t *sinfo, void *context) {
	char msg[60];
	sprintf(msg, "I captured a SIGUSR1 sent by the process with PID %d.\n", sinfo->si_pid);
	write(STDOUT_FILENO, msg, strlen(msg));
}

int main() {
	
	struct sigaction act;

	memset(&act, 0, sizeof(struct sigaction));
	sigemptyset(&act.sa_mask);
	act.sa_sigaction = handle_USR1;
	act.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR1, &act, NULL);
	
	pid_t p = fork();
	pid_t parent_p = getppid();

	if (p == -1) {
		perror("Fork failed!");
		exit(1);
	}
	
	for ( ; ; ) {
		
		if (p > 0) {
			printf("I'm running! :)\n");
			pause();
		} else {
			sleep(2);
			kill(parent_p, SIGUSR1);
		}
		
	}

	return 0;
	
}
