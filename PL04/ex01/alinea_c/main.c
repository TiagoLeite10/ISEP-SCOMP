#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>

#include "../../libs/my_semaphore_lib.h"

#define ERROR_VALUE -1
#define SEMAPHORE_NAME "/sem_ex01c_%d"
#define NUMBER_OF_PROCESSES 8
#define NUMBER_OF_SEMAPHORES 8
#define TOTAL_NUMBERS 200
#define MAX_LENGTH 20
#define INPUT_FILE "Numbers.txt"
#define OUTPUT_FILE "Output.txt"

int main() {
	FILE *input, *output;
    int i, j, num, status;
    pid_t p;

    sem_t *sem[NUMBER_OF_SEMAPHORES];
    char semaphores_name[NUMBER_OF_SEMAPHORES][MAX_LENGTH];
    int initial_values[NUMBER_OF_SEMAPHORES] = {1, 0, 0, 0, 0, 0, 0, 0};

    /* Cria os semáforos */
    for (i = 0; i < NUMBER_OF_SEMAPHORES; i++) {
        sprintf(semaphores_name[i], SEMAPHORE_NAME, i);
        sem[i] = semaphore_open_with_validation(semaphores_name[i], O_CREAT | O_EXCL, 0644, initial_values[i]);
	}

    /* Apaga a última versão do ficheiro Output.txt */
    remove(OUTPUT_FILE);

    /* Cria os processos filhos */
    for (i = 0; i < NUMBER_OF_PROCESSES; i++) {
        p = fork();

        /* Caso aconteça uma falha na criação do processo filho */
        if (p == ERROR_VALUE) {
            perror("Fork failed!\n");
            exit(EXIT_FAILURE);
        }

        if (p == 0) { 

            /* Espera até que o semáforo tenha um valor maior que zero, para então decrementá-lo */
            sem_wait_with_validation(sem[i]);

            /* Abre o ficheiro Numbers.txt */
            input = fopen(INPUT_FILE, "r");
            if (input == NULL) {
                printf("A error occurred while trying to open the file!\n");
                exit(EXIT_FAILURE);
            }

            /* Abre o ficheiro Output.txt */
            output = fopen(OUTPUT_FILE, "a");
            if (output == NULL) {
                printf("A error occurred while trying to open the file!\n");
                exit(EXIT_FAILURE);
            }

            /* Lê e escreve os números e o seu pid num ficheiro */
            for (j = 0; j < TOTAL_NUMBERS; j++) {
                fscanf(input, "%d", &num);

                fprintf(output, "[%d] %d\n", getpid(), num);      
            }

            /* Fecha o ficheiro Numbers.txt */
            if (fclose(input)) {
                printf("A error occurred while trying to close the file!\n");
                exit(EXIT_FAILURE);
            }

            /* Fecha o ficheiro Output.txt */
            if (fclose(output)) {
                printf("A error occurred while trying to close the file!\n");
                exit(EXIT_FAILURE);
            }

            printf("Filho nº %d com o pid %d leu e escreveu os %d números!\n", i + 1, getpid(), TOTAL_NUMBERS);

            if (i < NUMBER_OF_PROCESSES - 1) {
                /* Incrementa e liberta o semáforo */
                sem_post_with_validation(sem[i + 1]); 
            }

            /* Se não der erro fecha os semáforos */
            for (i = 0; i < NUMBER_OF_SEMAPHORES; i++) {
                semaphore_close_with_validation(sem[i]);
            }

            exit(EXIT_SUCCESS);
        }
    }
    
    /* Esperar que os processos filhos terminem */
    for (i = 0; i < NUMBER_OF_PROCESSES; i++) {
        if (wait(&status) == ERROR_VALUE) {
            perror("Something went wrong while waiting for my child process child to terminate!\n");
            exit(EXIT_FAILURE);
        }

        /* Se o processo filho não terminar com sucesso */
        if (!WIFEXITED(status)) {
            perror("The child process did not finish successfully!\n");
            exit(EXIT_FAILURE);
        }
    }

    /* Se não der erro fecha e remove os semáforos */
    for (i = 0; i < NUMBER_OF_SEMAPHORES; i++) {
        semaphore_close_with_validation(sem[i]);
        semaphore_unlink_with_validation(semaphores_name[i]);
    }

	return 0;
}