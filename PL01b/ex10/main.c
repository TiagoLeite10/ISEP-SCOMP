#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define BURST_SIZE 12

volatile sig_atomic_t USR1_counter = 0;

void handle_USR1(int signo) {
	
	sleep(2);

	char msg[50];
	USR1_counter++;
	
	sprintf(msg, "SIGUSR1 signal captured: USR1_counter = %d\n", USR1_counter);
	write(STDOUT_FILENO, msg, strlen(msg));
}

int main() {

	struct sigaction act;
	pid_t p;

	memset(&act, 0, sizeof(struct sigaction));
	//sigfillset(&act.sa_mask); // Bloquear todos os sinais
	sigemptyset(&act.sa_mask); // alínea E

	act.sa_flags = SA_NODEFER; // alínea E
	act.sa_handler = handle_USR1;
	sigaction(SIGUSR1, &act, NULL);

	p = fork();

	if (p > 0) {
    		for ( ; ; ) {
    			printf("I'm working!\n");
    			sleep(1);
    		}
	} else {
		struct timespec req;
		req.tv_sec = 0;
		req.tv_nsec = 10000000;
		
		pid_t parent_pid = getppid();

		for (int i = 0; i < BURST_SIZE; i++) {
			kill(parent_pid, SIGUSR1);
			nanosleep(&req, NULL);
		}

		kill(parent_pid, SIGINT);
	}

	return 0;
}

