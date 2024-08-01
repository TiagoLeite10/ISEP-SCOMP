#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "generate_random_values.h"

#define ARRAY_SIZE 1000
#define LIMIT_NUMBER 10
#define NUMBER_CHILDREN 5
#define READ 0
#define WRITE 1

int main() {
	int vec1[ARRAY_SIZE], vec2[ARRAY_SIZE], result[ARRAY_SIZE], fd[5][2];	
	int k = 0, i, j;
	pid_t p;

	/* Gerar valores aleatórios para os vetores 1 e 2 */
    generate_random_values(vec1, ARRAY_SIZE, LIMIT_NUMBER);
	generate_random_values(vec2, ARRAY_SIZE, LIMIT_NUMBER);

	int numbers_analyze_child = ARRAY_SIZE / NUMBER_CHILDREN;
	
	/* Cria 5 processos filhos para somar simultaneamente os dois arrays e enviar o seu resultado para o processo pai através de pipes */
	for (i = 0; i < NUMBER_CHILDREN; i++) {

		/* Cria o pipe */
		if (pipe(fd[i]) == -1) {
			perror("Falha no pipe");
			exit(EXIT_FAILURE);
		}

	    /* Cria um processo filho (cliente) */
		p = fork();

		/* Caso aconteça uma falha na criação do processo filho */
		if (p == -1) {
			perror("Fork falhou");
			exit(EXIT_FAILURE);
		}
	
		if (p == 0) {
			/* Fecha a extremidade não usada */
			close(fd[i][READ]);

		    int initial_index = i * numbers_analyze_child;
            int final_index =  initial_index + numbers_analyze_child;

			/* Escreve no pipe */
            for (j = initial_index; j < final_index; j++) {
				int total = vec1[j] + vec2[j];

				write(fd[i][WRITE], &total, sizeof(total));
            }
			
			/* Fecha a extremidade usada */
			close(fd[i][WRITE]);

			exit(EXIT_SUCCESS);
		}
	}

	/* O processo pai armazena os 1000 valores em uma matriz de resultados */
	for (i = 0; i < NUMBER_CHILDREN; i++) {
		/* Fecha a extremidade não usada */
		close(fd[i][WRITE]);

		/* Lê dados do pipe */
		for (j = 0; j < ARRAY_SIZE / NUMBER_CHILDREN; j++) {
			read(fd[i][READ], &result[k], sizeof(result[k]));
			k++;
		}

     	/* Fecha a extremidade usada */
		close(fd[i][READ]);
	}

	/* Imprime a matriz de resultados */
	for (i = 0; i < ARRAY_SIZE; i++) {
		printf("%d + %d = %d \n", vec1[i], vec2[i], result[i]);	
	}

	return 0;
}