#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "spawn_childs.h"

#define N_CHILDS_TO_SPAWN 6

int main() {

	int status;

	int x = spawn_childs(N_CHILDS_TO_SPAWN);

	if (x == 0) {
		for (int i = 0; i < N_CHILDS_TO_SPAWN; i++) {
			wait(&status);
			printf("Um processo filho terminou com o valor %d.\n", WEXITSTATUS(status));
		}

		printf("Sou o processo pai, e vou terminar agora, após todos os meus processos filhos terem terminado.\n");
	} else {
		int exit_value = x << 1; // exit_value = x * 2;
		printf("Olá, eu sou o filho número %d, e vou terminar com o valor %d.\n", x, exit_value);
		exit(exit_value);
	}

	return 0;
}
