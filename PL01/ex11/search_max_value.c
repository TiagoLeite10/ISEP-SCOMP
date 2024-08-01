#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* Função para criar um determinado número de processos filhos que encontrarão concorrentemente o valor máximo do array */
int search_max_value(int *numbers, int size, int number_children) {
    int i, j, status, max_value = 0;
    int numbers_analyze_child = size / number_children;
    pid_t p;
    
    for (i = 0; i < number_children; i++) {
        p = fork();

        /* Caso aconteça uma falha na criação do processo filho */
        if (p == -1) {
            perror("Fork falhou");
            exit(EXIT_FAILURE);
        }

        if (p == 0) {
            int initial_index = i * numbers_analyze_child;
            int final_index =  initial_index + numbers_analyze_child;

            for(j = initial_index; j < final_index; j++) {
                if (numbers[j] > max_value) {
                    max_value = numbers[j];
                }
            }

            exit(max_value);        
        }
    }

    // Determinar o número máximo encontrado pelos 5 filhos
    for (i = 0; i < number_children; i++) {
        wait(&status);

        if (WIFEXITED(status)){
            max_value =  WEXITSTATUS(status) > max_value ? WEXITSTATUS(status) : max_value;	
        }
    }

    return max_value;
}