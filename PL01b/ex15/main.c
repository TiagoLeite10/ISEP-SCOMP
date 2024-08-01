#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "command.h"

#define NUMBER_COMMANDS 3

volatile sig_atomic_t p;
command_t last;

void handle_ALRM(int signo, siginfo_t *sinfo, void *context) {
	kill(p, SIGKILL);
	
	char msg[100];
	sprintf(msg, "The command %s did not complete in its allowed time! \n", last.cmd);
	write(STDOUT_FILENO, msg, strlen(msg));
}

int main() {
	struct sigaction act;
	
	memset(&act, 0, sizeof(struct sigaction));
	sigemptyset(&act.sa_mask);
	act.sa_sigaction = handle_ALRM;
	act.sa_flags = SA_SIGINFO;

	sigaction(SIGALRM, &act, NULL);

	command_t c[NUMBER_COMMANDS];
	strcpy(c[0].cmd, "./programs/prog1");
	strcpy(c[1].cmd, "./programs/prog2");
	strcpy(c[2].cmd, "./programs/prog3");

	c[0].time_cap = 8;
	c[1].time_cap = 6;
	c[2].time_cap = 3;

	// Ciclo for para percorrer o array de comandos. O processo filho é responsável por executar o programa e o processo pai por controlar o tempo limite de execução.
	for(int i = 0; i < NUMBER_COMMANDS; i++) {
		last = c[i];
		p = fork();

		/* Caso aconteça uma falha na criação do processo filho */
		if (p == -1) {
			perror("Fork falhou");
			exit(EXIT_FAILURE);
		}

		if (p == 0) {
			execlp(c[i].cmd, c[i].cmd, NULL); // A função execlp substitui o filho pelo programa executado
			exit(-1); // Significa que o exec falhou
		} else if (p > 0) {
			alarm(c[i].time_cap);
			waitpid(p, NULL, 0);
		}
	}

    return 0;
}