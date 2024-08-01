#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include <stdlib.h>

#define NUM_OF_CHILDS 5

volatile sig_atomic_t childs_executing = 0;

void handler_USR1(int signo) {
	childs_executing--;
}

int main() {
	struct sigaction act;
	memset(&act, 0, sizeof(struct sigaction));
	act.sa_flags = SA_NOCLDWAIT | SA_NOCLDSTOP;
	sigaction(SIGCHLD, &act, NULL);

	struct sigaction act_two;
	memset(&act_two, 0, sizeof(struct sigaction));
	sigfillset(&act_two.sa_mask);
	act.sa_handler = handler_USR1;
	sigaction(SIGUSR1, &act_two, NULL);

	pid_t p[NUM_OF_CHILDS] = {0};
	int i, is_child = 0;
	childs_executing = NUM_OF_CHILDS;

	for (i = 0; i < NUM_OF_CHILDS; i++) {
		p[i] = fork();

		if (p[i] == 0) {
			is_child = 1;
			break;
		}
	}

	if (is_child == 1) {
		pid_t parent_p = getppid();
		int initial_value = i << 7; // i * 126
		initial_value += i << 6; // initial_value += i * 64
		initial_value += i << 3; // initial_value += i * 8 (no total: initial_value = i * 200)
		i += 5;
		int final_value = i << 7;
		final_value += i << 6;
		final_value += i << 3;

		for (int j = initial_value; j < final_value; j++) {
			printf("%d\n", j);
		}

		kill(parent_p, SIGUSR1);
		exit(0);
	} else {

		while (childs_executing != 0) {
			pause();
		}

		for (int j = 0; j < NUM_OF_CHILDS; j++) {
			waitpid(p[j], NULL, 0);
		}
	}

	return 0;
}

