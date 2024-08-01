#include <stdio.h>
#include <signal.h>

int main(void) {
	int pid, sinal;

	printf("PID alvo: ");
	scanf("%d", &pid);

	printf("Número do sinal a enviar: ");
	scanf("%d", &sinal);

	kill(pid, sinal);

	printf("O programa com o PID %d recebeu o sinal número %d!\n", pid, sinal);

	return 0;
}