#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    int status;
    pid_t p1 = fork();

    /* Caso aconteça uma falha na criação do processo filho 1 */
	if (p1 == -1) {
		perror("Fork falhou");
		exit(EXIT_FAILURE);
	}

    if (p1 == 0) {
        sleep(1);
        exit(1);
    } else if (p1 > 0) {
        pid_t p2 = fork();

        /* Caso aconteça uma falha na criação do processo filho 2 */
        if (p2 == -1) {
            perror("Fork falhou");
            exit(EXIT_FAILURE);
        }

        if (p2 == 0) {
            sleep(2);
            exit(2);
        } else if (p2 > 0) {
        
            waitpid(p1, &status, 0);
            if (WIFEXITED(status)) {
                printf("Valor retornado pelo filho 1: %d\n", WEXITSTATUS(status));
            }

            waitpid(p2, &status, 0);
            if (WIFEXITED(status)) {
                printf("Valor retornado pelo filho 2: %d\n", WEXITSTATUS(status));
            }    
        }
    }
}