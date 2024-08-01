#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MSG_SIZE 20
#define READ 0
#define WRITE 1

int main() {	
	char read_msg[MSG_SIZE] = "", write_msg[MSG_SIZE] = "";
	int fd[2], status;
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
		while (read(fd[READ], read_msg, MSG_SIZE) > 0) {
            printf("Mensagem lida pelo filho: %s\n", read_msg);
        }
		
		/* Fecha a extremidade usada */
		close(fd[READ]);

		exit(EXIT_SUCCESS);
	} else if (p > 0) {
		/* Fecha a extremidade não usada */
		close(fd[READ]);

		/* Escreve no pipe */
		strcpy(write_msg, "Hello World");
		write(fd[WRITE], write_msg, MSG_SIZE);
		strcpy(write_msg, "Goodbye!");
		write(fd[WRITE], write_msg, MSG_SIZE);

		/* Fecha a extremidade usada */
		close(fd[WRITE]);	

		/* Espera que o processo filho termine */
		wait(&status);
		if (WIFEXITED(status)) {
			printf("O filho com o pid %d retornou o valor de saída %d!\n", p, WEXITSTATUS(status));
		}
	} 

	return 0;
}