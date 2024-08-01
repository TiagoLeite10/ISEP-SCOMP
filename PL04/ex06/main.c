#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <semaphore.h>

#include "../libs/my_semaphore_lib.h"

#define PARENT_SEMAPHORE_NAME "/ex06_parent_semaphore"
#define CHILD_SEMAPHORE_NAME "/ex06_child_semaphore"
#define ERROR_VALUE -1

int main() {
	// Cria o semáforo para controlar o processo pai
	sem_t *parent_control_semaphore = semaphore_open_with_validation(PARENT_SEMAPHORE_NAME, O_CREAT | O_EXCL, 0644, 2);
	
	// Cria o semáforo para controlar o processo filho
	sem_t *child_control_semaphore = semaphore_open_with_validation(CHILD_SEMAPHORE_NAME, O_CREAT | O_EXCL, 0644, 2);

	pid_t p = fork();
	/* Caso aconteça uma falha na criação do processo filho */
	if (p == ERROR_VALUE) {
		perror("Fork failed!\n");
		exit(EXIT_FAILURE);
	}

	if (p > 0) { // Se for o processo pai
		while (1) {
			sem_wait_with_validation(parent_control_semaphore); // Subtrai 1 ao semáforo que controla a escrita no processo pai
			printf("S");
			fflush(stdout);
			sem_post_with_validation(child_control_semaphore); // Soma 1 ao semáforo que controla a escrita do processo filho
		}
		
	} else { // Se for o processo filho
		while (1) {
			sem_wait_with_validation(child_control_semaphore); // Subtrai 1 ao semáforo que controla a escrita no processo filho
			printf("C");
			fflush(stdout);
			sem_post_with_validation(parent_control_semaphore); // Soma 1 ao semáforo que controla a escrita do processo pai
		}
	}

	return 0;
}
