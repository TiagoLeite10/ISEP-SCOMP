#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define SECONDS_TO_INSERT_SENTENCE 10 // Segundos em que é possível inserir a frase
#define STRING_MAX_LENGTH 100 // Tamanho máximo da string a ser digitada

void handle_CHILD(int signo) {
	write(STDOUT_FILENO, "\nYou were too slow and this program will end!\n", 46);
	exit(0);
}

int main() {
	char sentence[STRING_MAX_LENGTH];
	struct sigaction act;

	memset(&act, 0, sizeof(struct sigaction));
	act.sa_handler = handle_CHILD;
	sigaction(SIGCHLD, &act, NULL);
	sigfillset(&act.sa_mask); // Bloquear todos os sinais

	pid_t p = fork();

	if (p > 0) {
		printf("Insert a sentence (you only have 10 seconds to do it, otherwise the program will terminate): \n");
		fgets(sentence, sizeof(sentence), stdin);
		
		printf("Tamanho da frase inserida: %ld\n", strlen(sentence) - 1); // - 1 por causa do \n lido pelo fgets
		kill(p, SIGKILL);
	} else {
		sleep(SECONDS_TO_INSERT_SENTENCE);
	}

	return 0;

}
