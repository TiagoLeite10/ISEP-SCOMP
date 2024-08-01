#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define SECONDS_TO_ALARM 10
#define STRING_MAX_LENGTH 100

void handle_ALARM(int signo) {
	write(STDOUT_FILENO, "\nYou were too slow and this program will end!\n", 46);
	exit(0);
}

int main() {
	char sentence[STRING_MAX_LENGTH];
	struct sigaction act;

	memset(&act, 0, sizeof(struct sigaction));
	act.sa_handler = handle_ALARM;
	sigaction(SIGALRM, &act, NULL);
	sigfillset(&act.sa_mask); // Bloquear todos os sinais

	printf("Insert a sentence (you only have 10 seconds to do it, otherwise the program will terminate): \n");
	alarm(SECONDS_TO_ALARM);
	fgets(sentence, sizeof(sentence), stdin);

	printf("Tamanho da frase inserida: %ld\n", strlen(sentence) - 1); // - 1 por causa do \n lido pelo fgets

	return 0;

}
