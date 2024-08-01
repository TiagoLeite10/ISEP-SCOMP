#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int spawn_childs(int n) {

	pid_t p;
	int index = 0;

	for (int i = 0; i < n; i++) {
	
		p = fork();
		
		if (p == -1) {
			perror("Ocorreu um erro ao criar o processo filho.");
			exit(1);
		}

		if (p == 0) {
			index = i + 1;
			break;
		}

	}
	
	return index;
}
