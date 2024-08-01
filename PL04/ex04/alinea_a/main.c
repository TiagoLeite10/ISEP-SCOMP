#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>

#include "../../libs/my_semaphore_lib.h"

#define ERROR_VALUE -1
#define SEMAPHORE_NAME "/sem_ex04a_%d"
#define SEMAPHORE_INDEX_1 0
#define SEMAPHORE_INDEX_2 1
#define NUMBER_OF_PROCESSES 2
#define NUMBER_OF_SEMAPHORES 2
#define MAX_LENGTH 20

int main() {
    int i, status;
    pid_t p;

    sem_t *sem[NUMBER_OF_SEMAPHORES];
    char semaphores_name[NUMBER_OF_SEMAPHORES][MAX_LENGTH];
	int initial_values[NUMBER_OF_SEMAPHORES] = {0, 0};

    /* Cria os semáforos */
    for (i = 0; i < NUMBER_OF_SEMAPHORES; i++) {
        sprintf(semaphores_name[i], SEMAPHORE_NAME, i);
        sem[i] = semaphore_open_with_validation(semaphores_name[i], O_CREAT | O_EXCL, 0644, initial_values[i]);
	}

    /* Cria os processos filhos */
    for (i = 0; i < NUMBER_OF_PROCESSES; i++) {
        p = fork();

        /* Caso aconteça uma falha na criação do processo filho */
        if (p == ERROR_VALUE) {
            perror("Fork failed!\n");
            exit(EXIT_FAILURE);
        }

        if (p == 0) {
            if (i == 0) {
                printf("1st child\n");

                /* Incrementa e liberta o próximo semáforo */
                sem_post_with_validation(sem[SEMAPHORE_INDEX_1]);
            
            } else if (i == 1) {
                /* Espera até que o semáforo tenha um valor maior que zero, para então decrementá-lo */
                sem_wait_with_validation(sem[SEMAPHORE_INDEX_2]);

                printf("2nd child\n");
            }

            /* Se não der erro fecha os semáforos */
            for (i = 0; i < NUMBER_OF_SEMAPHORES; i++) {
                semaphore_close_with_validation(sem[i]);
            }

            exit(EXIT_SUCCESS);        
        }
    }

    /* O processo pai espera até que o semáforo tenha um valor maior que zero, para então decrementá-lo */
    sem_wait_with_validation(sem[SEMAPHORE_INDEX_1]);

    printf("Father\n");

    /* Incrementa e liberta o próximo semáforo */
    sem_post_with_validation(sem[SEMAPHORE_INDEX_2]);
    
    /* Espera que os processos filhos terminem */
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