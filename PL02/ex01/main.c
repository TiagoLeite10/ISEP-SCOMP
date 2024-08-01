#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define READ 0
#define WRITE 1

int main() {	
	int read_msg, write_msg;
	int fd[2];
	pid_t p;
	
	/* Cria o pipe */
	if (pipe(fd) == -1) {
		perror("Falha no pipe");
		exit(EXIT_FAILURE);
	}

	/* Cria um processo filho */
	p = fork();

	/* Caso aconteça uma falha na criação do processo filho */
	if (p == -1) {
		perror("Fork falhou");
		exit(EXIT_FAILURE);
	}

	if (p == 0) {
		/* Fecha a extremidade não usada */
		close(fd[WRITE]);

		/* Lê dados do pipe */
		read(fd[READ], &read_msg, sizeof(read_msg));		
		printf("Pid do pai lido pelo filho: %d\n", read_msg);
		
		/* Fecha a extremidade usada */
		close(fd[READ]);
	} else if (p > 0) {
		write_msg = (int) getpid();
		/* O proceso pai imprime o seu o pid */
		printf("Pid do pai: %d\n", write_msg);	
		
		/* Fecha a extremidade não usada */
		close(fd[READ]);

		/* Escreve no pipe */
		write(fd[WRITE], &write_msg, sizeof(write_msg));

		/* Fecha a extremidade usada */
		close(fd[WRITE]);	
	}

	return 0;
}