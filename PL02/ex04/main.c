#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define READ 0
#define WRITE 1
#define FILE_PATH "./exercicio.txt"

int main() {	
	
	int fd[2];
	pid_t p;
	
	// Criação do pipe
	if (pipe(fd) == -1) {
		perror("Pipe failed");
		return 1;
	}

	// Criar o processo filho
	p = fork();
	if (p == -1) {
		perror("Fork failed");
		return 1;
	}

	// Se for o pai
	if (p > 0) {
		close(fd[READ]);

		FILE* ptr;
		ptr = fopen(FILE_PATH, "r");

		if (ptr == NULL) {
			perror("File can't be opened!\n");
			kill(p, SIGKILL);
			return 1;
		}

		char file_char;
		
		// Ler caracter a caracter do ficheiro e escrevê-lo no pipe
		while (!feof(ptr)) {
			file_char = fgetc(ptr);
			if (file_char != EOF)
				write(fd[WRITE], &file_char, sizeof(char));
		}

		fclose(ptr);
		close(fd[WRITE]);
		waitpid(p, NULL, 0);
	} else {
		close(fd[WRITE]);
		
		size_t pipe_read_size;
		char read_pipe_char;

		// Ler caracter a caracter do pipe e escrevê-lo no ecrã
		while ((pipe_read_size = read(fd[READ], &read_pipe_char, sizeof(char))) > 0) {
			printf("%c", read_pipe_char);
		}

		// Se o tamanho do valor lido do pipe for -1 significa que ocorreu um erro
		if (pipe_read_size != 0) {
			perror("Something went wrong while reading from pipe!");
			return 1;
		}
		
		close(fd[READ]);
	}

	return 0;

}
