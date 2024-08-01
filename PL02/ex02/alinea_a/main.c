#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define READ 0
#define WRITE 1
#define BUFFER_SIZE 50

int main() {	
	int fd[2];
	pid_t p;

	int read_int;
	int write_int = 20;

	char read_msg[BUFFER_SIZE];
	char write_msg[BUFFER_SIZE] = "Mensagem escrita por pipe! Funcionou!";

	// Criar o pipe
	if (pipe(fd) == -1) {
		perror("Pipe failed");
		return 1;
	}

	p = fork();
	if (p == -1) {
		perror("Fork failed");
		return 1;
	}

	if (p > 0) {
		close(fd[WRITE]); // Fecha a extremidade de escrita (não utilizada)
		
		read(fd[READ], &read_int, sizeof(read_int));
		printf("Valor inteiro lido do pipe: %d\n", read_int);

		read(fd[READ], read_msg, BUFFER_SIZE);
		printf("String lida do pipe: \"%s\"\n", read_msg);

		close(fd[READ]); // Fecha a extremidade de leitura
	} else {
		close(fd[READ]);

		write(fd[WRITE], &write_int, sizeof(write_int));
		write(fd[WRITE], write_msg, strlen(write_msg) + 1);

		close(fd[WRITE]);
	}

	return 0;
}
