#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#include "../../libs/my_shared_memory_lib.h"
#include "../../libs/my_semaphore_lib.h"
#include "generate_random_value.h"
#include "shared_data_type.h"

#define ERROR_VALUE -1
#define SHARED_MEMORY_NAME "/ex07b"
#define SEMAPHORE_NAME "/sem_ex07b_%d"
#define SEMAPHORE_INDEX_NPROC 0
#define SEMAPHORE_INDEX_BARRIER 1
#define NUMBER_OF_PROCESSES 6
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

/* Representa a ação de realizar uma compra aleatória */
void exec_buy_random(int option) {
	if (option % 2 == 0) {
        buy_chips();
    } else {
        buy_beer();
    }
}

int main() {
    int data_size = sizeof(shared_data_type);
    int i, random_number, status;
    pid_t p;    
    
    sem_t *sem[NUMBER_OF_SEMAPHORES];
    char semaphores_name[NUMBER_OF_SEMAPHORES][MAX_LENGTH];
	int initial_values[NUMBER_OF_SEMAPHORES] = {1, 0};

    /* Cria e abre a zona de memória partilhada */
    int fd = shm_open_with_validations(SHARED_MEMORY_NAME, O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);

    /* Se não der erro no ftruncate define o tamanho da área da memória partilhada e inicializa-a a 0 */
    ftruncate_with_validations(fd, data_size);

    /* Pointer para a memória partilhada */
	shared_data_type *shared_data = (shared_data_type*)mmap_with_validations(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    /* Cria os semáforos */
    for (i = 0; i < NUMBER_OF_SEMAPHORES; i++) {
        sprintf(semaphores_name[i], SEMAPHORE_NAME, i);
        sem[i] = semaphore_open_with_validation(semaphores_name[i], O_CREAT | O_EXCL, 0644, initial_values[i]);
	}

    /* Cria os processos filhos */
    for (i = 0; i < NUMBER_OF_PROCESSES; i++) {
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

            /* Realiza uma compra aleatória */
            exec_buy_random(random_number);
            
            /* Espera até que o semáforo tenha um valor maior que zero, para então decrementá-lo */
            sem_wait_with_validation(sem[SEMAPHORE_INDEX_NPROC]);
            
            /* Incrementa o contador de processos à espera da barreira */
            shared_data->nproc_at_barrier++;
            
            /* Incrementa e liberta o semáforo */
            sem_post_with_validation(sem[SEMAPHORE_INDEX_NPROC]);

            /* Verifica se todos os processos chegaram à barreira */
            if (shared_data->nproc_at_barrier == NUMBER_OF_PROCESSES) {
                sem_post_with_validation(sem[SEMAPHORE_INDEX_BARRIER]);
            }

            /* Espera na barreira */
            sem_wait_with_validation(sem[SEMAPHORE_INDEX_BARRIER]);

            /* Permite que o último processo também continue */
            sem_post_with_validation(sem[SEMAPHORE_INDEX_BARRIER]);
            
            /* Come e bebe */
            eat_and_drink();

            /* Se não der erro desconecta o pointer da memória partilhada */
            munmap_with_validations(shared_data, data_size);

            /* Se não der erro fecha a zona de memória partilhada */
            close_with_validations(fd);

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

        /* Se o processo filho não terminar com sucesso*/
        if (!WIFEXITED(status)) {
            perror("The child process did not finish successfully!\n");
            exit(EXIT_FAILURE);
        }
    }

    /* Se não der erro desconecta o pointer da memória partilhada */
    munmap_with_validations(shared_data, data_size);

    /* Se não der erro fecha a zona de memória partilhada */
    close_with_validations(fd);

	/* Se não der erro remove a zona de memória partilhada */
    shm_unlink_with_validations(SHARED_MEMORY_NAME);

    /* Se não der erro fecha e remove os semáforos */
    for (i = 0; i < NUMBER_OF_SEMAPHORES; i++) {
        semaphore_close_with_validation(sem[i]);
        semaphore_unlink_with_validation(semaphores_name[i]);
    }

	return 0;
}