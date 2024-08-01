#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>

#include "../../libs/my_semaphore_lib.h"
#include "generate_random_value.h"

#define ERROR_VALUE -1
#define SEMAPHORE_NAME "/sem_ex07_%d"
#define SEMAPHORE_INDEX_1 0
#define SEMAPHORE_INDEX_2 1
#define NUMBER_OF_SEMAPHORES 2
#define INITIAL_LIMIT 1
#define FINAL_LIMIT 10
#define MAX_LENGTH 20

/* Representa a ação de comprar batatas fritas */
void buy_chips() {
    printf("[%d] Buy chips!\n", getpid());
}

/* Representa a ação de comprar cerveja */
void buy_beer() {
    printf("[%d] Buy beer!\n", getpid());
}

/* Representa a ação de comer e beber */
void eat_and_drink() {
    printf("[%d] Eat and Drink!\n", getpid());
}

int main() {
    int i, random_number, status;
    pid_t p;    
    
    sem_t *sem[NUMBER_OF_SEMAPHORES];
    char semaphores_name[NUMBER_OF_SEMAPHORES][MAX_LENGTH];

    /* Cria os semáforos */
    for (i = 0; i < NUMBER_OF_SEMAPHORES; i++) {
        sprintf(semaphores_name[i], SEMAPHORE_NAME, i);
        sem[i] = semaphore_open_with_validation(semaphores_name[i], O_CREAT | O_EXCL, 0644, 0);
	}

    random_number = generate_random_value(INITIAL_LIMIT, FINAL_LIMIT);

    p = fork();

    /* Caso aconteça uma falha na criação do processo filho */
    if (p == ERROR_VALUE) {
        perror("Fork failed!\n");
        exit(EXIT_FAILURE);
    }

    /* Processo filho */
    if (p == 0) {
        /* Dorme um tempo aleatório */
        sleep(random_number);

        /* Compra batatas fritas */
        buy_chips();
        
        /* Incrementa e liberta o semáforo do pai */
        sem_post_with_validation(sem[SEMAPHORE_INDEX_1]);
        /* Espera até que o semáforo tenha um valor maior que zero, para então decrementá-lo */
        sem_wait_with_validation(sem[SEMAPHORE_INDEX_2]);

        /* Come e bebe */
        eat_and_drink();

        /* Se não der erro fecha os semáforos */
        for (i = 0; i < NUMBER_OF_SEMAPHORES; i++) {
            semaphore_close_with_validation(sem[i]);
        }

        exit(EXIT_SUCCESS);

    /* Processo pai */ 
    } else if (p > 0) {
        /* Dorme um tempo aleatório */
        sleep(random_number);

        /* Compra cerveja */
        buy_beer();

        /* Incrementa e liberta semáforo do filho */
        sem_post_with_validation(sem[SEMAPHORE_INDEX_2]);
        /* Espera até que o semáforo tenha um valor maior que zero, para então decrementá-lo */
        sem_wait_with_validation(sem[SEMAPHORE_INDEX_1]);

        /* Come e bebe */
        eat_and_drink();
    }    
        
    /* Espera que o processo filho termine */
    if (wait(&status) == ERROR_VALUE) {
        perror("Something went wrong while waiting for my child process child to terminate!\n");
        exit(EXIT_FAILURE);
    }

    /* Se o processo filho não terminar com sucesso*/
    if (!WIFEXITED(status)) {
        perror("The child process did not finish successfully!\n");
        exit(EXIT_FAILURE);
    }    

    /* Se não der erro fecha e remove os semáforos */
    for (i = 0; i < NUMBER_OF_SEMAPHORES; i++) {
        semaphore_close_with_validation(sem[i]);
        semaphore_unlink_with_validation(semaphores_name[i]);
    }

	return 0;
}