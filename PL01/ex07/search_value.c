#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* Função para criar um processo filho para descobrir quantas vezes o número n é encontrado na metade dos elementos do array de números. 
   Enquanto o filho está processando, o processo pai procura a outra metade. Depois que ambos terminam de processar a sua metade, 
   o pai calcula e retorna quantas vezes n foi encontrado em todo o array.
*/
int search_value(int *numbers, int size, int n) {
    int status, total_filho = 0, total_final = 0;
    pid_t p = fork();

    /* Caso aconteça uma falha na criação do processo filho */
	if (p == -1) {
		perror("Fork falhou");
		exit(EXIT_FAILURE);
	}

    if (p == 0) {        
        for (int i = 0; i < size/2; i++) {
            if (numbers[i] == n) {
                total_filho++;
            }
        }

        exit(total_filho);

    } else if (p > 0) {

        for (int i = size/2; i < size; i++) {
            if (numbers[i] == n) {
                total_final++;
            }
        }

        wait(&status);

        if (WIFEXITED(status)) {
            total_final = total_final + WEXITSTATUS(status);
        }
    }

    return total_final;
}