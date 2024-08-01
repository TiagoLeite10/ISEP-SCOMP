#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>

#include "../../libs/my_semaphore_lib.h"

#define ERROR_VALUE -1
#define SEMAPHORE_NAME "/sem_ex01b_%d"
#define SEMAPHORE_INDEX_1 0
#define SEMAPHORE_INDEX_2 1
#define NUMBER_OF_PROCESSES 8
#define NUMBER_OF_SEMAPHORES 2
#define TOTAL_NUMBERS 200
#define MAX_LENGTH 20
#define INPUT_FILE "Numbers.txt"
#define OUTPUT_FILE "Output.txt"

int main() {
	FILE *input, *output;
    int i, j, k, num, status;
    char line[MAX_LENGTH];
    pid_t p;

    sem_t *sem[NUMBER_OF_SEMAPHORES];
    char semaphores_name[NUMBER_OF_SEMAPHORES][MAX_LENGTH];

    /* Cria os semáforos */
    for (i = 0; i < NUMBER_OF_SEMAPHORES; i++) {
        sprintf(semaphores_name[i], SEMAPHORE_NAME, i);
        sem[i] = semaphore_open_with_validation(semaphores_name[i], O_CREAT | O_EXCL, 0644, 1);
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

            /* Lê e escreve os números e o seu pid num ficheiro */
            for (j = 0; j < TOTAL_NUMBERS; j++) {

                /* Espera até que o semáforo tenha um valor maior que zero, para então decrementá-lo */
                sem_wait_with_validation(sem[SEMAPHORE_INDEX_1]);

                printf("#Start critical section CS1\n");

                /* Abre o ficheiro Numbers.txt */
                input = fopen(INPUT_FILE, "r");
                if (input == NULL) {
                    printf("A error occurred while trying to open the file!\n");
                    exit(EXIT_FAILURE);
                }

                /*Lê e armazena a linha desejeda */         
                for (k = 0; k <= j; k++) {
                    fgets(line, MAX_LENGTH, input);
                }
                sscanf(line, "%d", &num);

                printf("    Read value %d from Numbers.txt\n", num);

                /* Fecha o ficheiro Numbers.txt */
                if (fclose(input)) {
                    printf("A error occurred while trying to close the file!\n");
                    exit(EXIT_FAILURE);
                }

                printf("#End critical section CS1\n");

                /* Incrementa e liberta o semáforo */
                sem_post_with_validation(sem[SEMAPHORE_INDEX_1]);  

                /* Espera até que o semáforo tenha um valor maior que zero, para então decrementá-lo */
                sem_wait_with_validation(sem[SEMAPHORE_INDEX_2]);

                printf("#Start critical section CS2\n");

                /* Abre o ficheiro Output.txt */
                output = fopen(OUTPUT_FILE, "a");
                if (output == NULL) {
                    printf("A error occurred while trying to open the file!\n");
                    exit(EXIT_FAILURE);
                }

                fprintf(output, "[%d] %d\n", getpid(), num);
                printf("    Write value %d to Output.txt\n", num);
            
                /* Fecha o ficheiro Output.txt */
                if (fclose(output)) {
                    printf("A error occurred while trying to close the file!\n");
                    exit(EXIT_FAILURE);
                }

                printf("#End critical section CS2\n\n");

                /* Incrementa e liberta o semáforo */
                sem_post_with_validation(sem[SEMAPHORE_INDEX_2]);             
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

     /* Abre o ficheiro Output.txt */
    output = fopen(OUTPUT_FILE, "r");
    if (output == NULL) {
        printf("A error occurred while trying to open the file!\n");
        exit(EXIT_FAILURE);
    }

    /* Imprime o conteúdo do ficheiro Output.txt */
    printf("File content: \n");
    while(fgets(line, MAX_LENGTH, output)) {
		printf(line);
	}

    /* Fecha o ficheiro Output.txt */
    if (fclose(output)) {
        printf("A error occurred while trying to close the file!\n");
        exit(EXIT_FAILURE);
    }

    /* Se não der erro fecha e remove os semáforos */
    for (i = 0; i < NUMBER_OF_SEMAPHORES; i++) {
        semaphore_close_with_validation(sem[i]);
        semaphore_unlink_with_validation(semaphores_name[i]);
    }

	return 0;
}