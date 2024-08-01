#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
	int x = 1;
	pid_t p = fork(); /*pid_t: sys/types.h; fork(): unistd.h*/

	/* Caso aconteça uma falha na criação do processo filho */
	if (p == -1) {
		perror("Fork falhou");
		exit(EXIT_FAILURE);
	}
	
	if (p == 0) {
		x = x + 1;
 		printf("1. x = %d\n", x);
 	} else if (p > 0) {
 		x = x - 1;
 		printf("2. x = %d\n", x);
 	}

 	printf("3. %d; x = %d\n", p, x);
}