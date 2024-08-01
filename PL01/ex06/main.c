#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#define SIZE 4

int main() {

	int i;
	int status;
	pid_t p[SIZE];
	int exit_code = 0;

	if (SIZE < 1) {
		printf("Este programa não tem as condições necessárias para ser executado.");
		return 0;
	}

	for (i = 0; i < SIZE; i++) {

		if ((p[i] = fork()) == 0) {
			sleep(1); /*sleep(): unistd.h*/
			exit_code = i + 1;
			break;
		}
	
	}

	if (exit_code == 0) {
		for (int j = 0; j < SIZE; j++) {
			if (p[j] % 2 == 0) {
				printf("Sou o processo pai, e estou à espera que o processo com o pid %d termine.\n", p[j]);
				waitpid(p[j], &status, 0);
				printf("O processo com o pid %d terminou! Este processo foi criado em %dº lugar\n", p[j], WEXITSTATUS(status));
			}
		}
	}

	printf("This is the end.\n");
	
	exit(exit_code);

}
