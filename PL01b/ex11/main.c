#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#define TOTAL_SIGNAL 64

void handle_USR1(int signo, siginfo_t *sinfo, void *context) {
	sigset_t sigset;
	sigprocmask(SIG_BLOCK, NULL, &sigset);

	write(STDOUT_FILENO, "Sinais bloqueados ao receber o SIGUSR1:\n", 40);
	for (int signal = 1; signal <= TOTAL_SIGNAL; signal++) {
		if (sigismember(&sigset, signal)) {
			char msg[4];
			sprintf(msg, "%d ", signal);
            write(STDOUT_FILENO, msg, strlen(msg));
		}
	}

	write(STDOUT_FILENO, "\n", 1);
}

int main() {	
	struct sigaction act;

	memset(&act, 0, sizeof(struct sigaction));
	sigfillset(&act.sa_mask); 
	act.sa_sigaction = handle_USR1;
	act.sa_flags = SA_SIGINFO;

	sigaction(SIGUSR1, &act, NULL);
	
	raise(SIGUSR1);

	return 0;
}