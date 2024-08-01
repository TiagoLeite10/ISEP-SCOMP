#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "generate_random_value.h"

#define NUMBER_CHILDREN 5
#define INITIAL_LIMIT 1
#define FINAL_LIMIT 500
#define READ 0
#define WRITE 1

int main() {
	int fd[NUMBER_CHILDREN + 1][2];
	int i, random_number, greater_number;
	pid_t p;

	/* Cria todos os pipes */
	for (i = 0; i <= NUMBER_CHILDREN; i++) {
		if (pipe(fd[i]) == -1) {
			perror("Falha no pipe");
			exit(EXIT_FAILURE);
		}
	}
	
	/* Gera um número aleatório e imprime junto com o PID do pai */
	greater_number = generate_random_value(INITIAL_LIMIT, FINAL_LIMIT);
	printf("Pai com o pid %d gerou o número aleatório: %d\n", getpid(), greater_number);

	for (i = 1; i <= NUMBER_CHILDREN; i++) {
		random_number =  generate_random_value(INITIAL_LIMIT, FINAL_LIMIT);

		p = fork();

		/* Caso aconteça uma falha na criação do processo filho */
		if (p == -1) {
			perror("Fork falhou");
			exit(EXIT_FAILURE);
		}

		if (p == 0) {			
			printf("Filho %d com o pid %d gerou o número aleatório: %d\n", i, getpid(), random_number);

			/* Fecha a extremidade não usada */
			close(fd[i - 1][WRITE]);
			close(fd[i][READ]);

			/* Lê dados do pipe anterior */
			read(fd[i - 1][READ], &greater_number, sizeof(greater_number));	
			
			if (random_number > greater_number) {
				greater_number = random_number;
			}

			/* Escreve no pipe atual */
			write(fd[i][WRITE], &greater_number, sizeof(greater_number));				
			
			/* Fecha a extremidade usada */
			close(fd[i - 1][READ]);
			close(fd[i][WRITE]);

			exit(EXIT_SUCCESS);
		
		} if (p > 0 && i == 1) {
			/* Fecha a extremidade não usada */
			close(fd[0][READ]);		
			
			/* Escreve no pipe */
			write(fd[0][WRITE], &greater_number, sizeof(greater_number));	
			
			/* Fecha a extremidade usada */
			close(fd[0][WRITE]);
		}
	}

	/* Fecha a extremidade não usada */
	close(fd[NUMBER_CHILDREN][WRITE]);
	/* Lê dados do pipe */
	read(fd[NUMBER_CHILDREN][READ], &greater_number, sizeof(greater_number));	
	/* Fecha a extremidade usada */
	close(fd[NUMBER_CHILDREN][READ]);

	printf("Maior número aleatório encontrado: %d\n", greater_number);

	return 0;
}