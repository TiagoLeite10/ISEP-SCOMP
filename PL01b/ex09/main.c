#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

void handle_signal(int signo, siginfo_t *sinfo, void *context) {
	if (signo == SIGINT) {
		write(STDOUT_FILENO, "\nI won’t let the process end with CTRL-C!\n", 44);
	} else if (signo == SIGQUIT) {
		write(STDOUT_FILENO, "\nI won’t let the process end by pressing CTRL-\\!\n", 51);
	}
}

int main() {
	struct sigaction act;

	memset(&act, 0, sizeof(struct sigaction));
	sigemptyset(&act.sa_mask);
	act.sa_sigaction = handle_signal;
	act.sa_flags = SA_SIGINFO;
	
	sigaction(SIGINT, &act, NULL);
	sigaction(SIGQUIT, &act, NULL);
	
	for ( ; ; ) {
		printf("I like signals \n");
		sleep(1);
	}

	return 0;
}