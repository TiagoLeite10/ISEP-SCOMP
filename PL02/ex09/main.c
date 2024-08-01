#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "sale.h"
#include "generate_random_sales.h"

#define ARRAY_SIZE 50000
#define NUMBER_CHILDREN 10
#define QUANTITY_LIMIT 20
#define READ 0
#define WRITE 1

int main() {
	sale sales[ARRAY_SIZE];
	int products[ARRAY_SIZE], fd[NUMBER_CHILDREN][2], k = 0, i, j;
	pid_t p;

	generate_random_sales(sales, ARRAY_SIZE);

	int numbers_analyze_child = ARRAY_SIZE / NUMBER_CHILDREN;

	/* Cria 10 processos filhos para saber quais produtos venderam mais de 20 unidades em uma única venda, o resultado é enviado para o processo pai através de pipes */
	for (i = 0; i < NUMBER_CHILDREN; i++) {
		
		/* Cria o pipe */
		if (pipe(fd[i]) == -1) {
			perror("Falha no pipe");
			exit(EXIT_FAILURE);
		}

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
				if (sales[j].quantity > QUANTITY_LIMIT) {
					int product_code = sales[j].product_code;
					write(fd[i][WRITE], &product_code, sizeof(product_code));
				}
            }
			
			/* Fecha a extremidade usada */
			close(fd[i][WRITE]);

			exit(EXIT_SUCCESS);
		
		} else if (p > 0) {
			/* Fecha a extremidade não usada */
			close(fd[i][WRITE]);

			/* Lê dados do pipe */
			while (read(fd[i][READ], &products[k], sizeof(products[k])) > 0) {
				k++;
			}
			
			/* Fecha a extremidade usada */
			close(fd[i][READ]);
		}
	}

	printf("Produtos que venderam mais de %d unidades em uma única venda\n", QUANTITY_LIMIT);
	for (i = 0; i < k; i++) {
		printf("Código do produto: %d\n", products[i]);
	}

	return 0;
}