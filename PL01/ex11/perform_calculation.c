#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* Função para o processo pai criar um processo filho para executar um cálculo na metade do array e imprimir o resultado.
   O processo pai realiza o mesmo cálculo na outra metade do array. A saída é ordenada pelos índices do array.
*/ 
void perform_calculation(int *numbers, int size, int max_value) {
    int result[size];
    int i, status;
    pid_t p = fork();

    /* Caso aconteça uma falha na criação do processo filho */
    if (p == -1) {
        perror("Fork falhou");
        exit(EXIT_FAILURE);
    }

    if (p == 0) {

        for (i = 0; i < size/2; i++) {
            result[i] = ((int) numbers[i]/max_value) * 100;

            printf("result[%d] = %d\n", i, result[i]);
        }    

    } else if (p > 0) {
        for (i = size/2; i < size; i++) {
            result[i] = ((int) numbers[i]/max_value) * 100;
        }  

        wait(&status);

        if (WIFEXITED(status)) {
            for (i = size/2; i < size; i++) {
                printf("result[%d] = %d\n", i, result[i]);
            }
        } 
    }
}