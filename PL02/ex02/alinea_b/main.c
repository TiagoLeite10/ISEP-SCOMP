#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define READ 0
#define WRITE 1
#define BUFFER_SIZE 50

struct my_struct {
	int number;
	char msg[BUFFER_SIZE];
};

int main() {	
	int fd[2];
	pid_t p;

	struct my_struct write_my_struct = {0}, read_my_struct = {0};

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
		
		read(fd[READ], &read_my_struct, sizeof(struct my_struct));
		printf("Valor inteiro lido do pipe: %d\n", read_my_struct.number);
		printf("String lida do pipe: \"%s\"\n", read_my_struct.msg);

		close(fd[READ]); // Fecha a extremidade de leitura
	} else {
		close(fd[READ]);

		write_my_struct.number = 20;
		strcpy(write_my_struct.msg, "Mensagem escrita por pipe! Funcionou!");
		write(fd[WRITE], &write_my_struct, sizeof(struct my_struct));

		close(fd[WRITE]);
	}

	return 0;
}
