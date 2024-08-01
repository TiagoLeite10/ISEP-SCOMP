#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>

#include "../libs/my_semaphore_lib.h"

#define ERROR_VALUE -1
#define SEMAPHORE_NAME "/sem_ex05_%d"
#define NUMBER_OF_PROCESSES 3
#define NUMBER_OF_SEMAPHORES 3
#define NUMBER_OF_WORDS 2
#define MAX_LENGTH 20

int main() {
    int i, j, status;
    pid_t p;    
    
    sem_t *sem[NUMBER_OF_SEMAPHORES];
    char semaphores_name[NUMBER_OF_SEMAPHORES][MAX_LENGTH];
	int initial_values[NUMBER_OF_SEMAPHORES] = {1, 0, 0};
    
    char* word_process_1[NUMBER_OF_WORDS] = {"Sistemas ", "a "};
	char* word_process_2[NUMBER_OF_WORDS] = {"de ", "melhor "};
	char* word_process_3[NUMBER_OF_WORDS] = {"Computadores - ", "disciplina!\n"};

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
            switch(i) {
                case 0:
                    for (j = 0; j < NUMBER_OF_WORDS; j++) {
                        /* Espera até que o semáforo tenha um valor maior que zero, para então decrementá-lo */
                        sem_wait_with_validation(sem[i]);

                        /* Imprime a palavra do processo 1 */
                        printf("%s", word_process_1[j]);
                        /* Para que a saída não seja armazenada no buffer */
                        fflush(stdout);

                        /* Incrementa e liberta o próximo semáforo */
                        sem_post_with_validation(sem[i + 1]);
                    }

                    break;
                case 1:
                    for (j = 0; j < NUMBER_OF_WORDS; j++) {
                        /* Espera até que o semáforo tenha um valor maior que zero, para então decrementá-lo */
                        sem_wait_with_validation(sem[i]);

                        /* Imprime a palavra do processo 2 */
                        printf("%s", word_process_2[j]);
                        /* Para que a saída não seja armazenada no buffer */
                        fflush(stdout);

                        /* Incrementa e liberta o próximo semáforo */
                        sem_post_with_validation(sem[i + 1]);
                    }

                    break;
                case 2:
                    for (j = 0; j < NUMBER_OF_WORDS; j++) {
                        /* Espera até que o semáforo tenha um valor maior que zero, para então decrementá-lo */
                        sem_wait_with_validation(sem[i]);

                        /* Imprime a palavra do processo 3 */
                        printf("%s", word_process_3[j]);
                        /* Para que a saída não seja armazenada no buffer */
                        fflush(stdout);

                        /* Incrementa e liberta o semáforo inicial para todo o processo repetir-se */
                        sem_post_with_validation(sem[i - 2]);
                    }

                   break;       
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