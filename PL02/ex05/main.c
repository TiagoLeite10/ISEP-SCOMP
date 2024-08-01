#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "uppercase.h"

#define MSG_SIZE 256
#define READ 0
#define WRITE 1

int main() {	
	char read_msg[MSG_SIZE] = "", write_msg[MSG_SIZE] = "";
	int up[2], down[2];
	pid_t p;
	
	/* Cria o pipe up */
	if (pipe(up) == -1) {
		perror("Falha no pipe up");
		exit(EXIT_FAILURE);
	}

	/* Cria o pipe down */
	if (pipe(down) == -1) {
		perror("Falha no pipe down");
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
		/* Fecha as extremidades não usadas */
		close(up[READ]);
		close(down[WRITE]);

		/* Escreve no pipe up */
		strcpy(write_msg, "Lower case");
		write(up[WRITE], write_msg, MSG_SIZE);

		/* Lê dados do pipe down */
		read(down[READ], read_msg, MSG_SIZE);
        printf("Mensagem enviada pelo servidor: %s\n", read_msg);        
		
		/* Fecha as extremidade usadas */
		close(up[WRITE]);
		close(down[READ]);

		exit(EXIT_SUCCESS);
	} else if (p > 0) {
		/* Fecha as extremidades não usadas */
		close(up[WRITE]);
		close(down[READ]);

		/* Lê dados do pipe up */
		read(up[READ], read_msg, MSG_SIZE);
		printf("Mensagem enviada pelo cliente: %s\n", read_msg);

		/* Escreve no pipe down */
		uppercase(read_msg);
		write(down[WRITE], read_msg, MSG_SIZE);      
		
		/* Fecha as extremidade usadas */
		close(up[READ]);
		close(down[WRITE]);	
	} 

	return 0;
}