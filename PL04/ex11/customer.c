#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#include "../libs/my_shared_memory_lib.h"
#include "../libs/my_semaphore_lib.h"
#include "generate_random_value.h"
#include "shared_data_type.h"

#define ERROR_VALUE -1
#define SHARED_MEMORY_NAME "/ex11"
#define SEMAPHORE_NAME "/sem_ex11_%d"
#define SEMAPHORE_INDEX_CLIENT 0
#define SEMAPHORE_INDEX_SELLER 1
#define NUMBER_OF_SEMAPHORES 2
#define INITIAL_LIMIT 1
#define FINAL_LIMIT 10
#define MAX_LENGTH 20

int main() {
    int data_size = sizeof(shared_data_type);
    int i, random;
   
    /* Espera 1 segundo para a main criar os recursos necessários */
    sleep(1);
	
    /* Abre a zona de memória partilhada já criada */
	int fd = shm_open_with_validations(SHARED_MEMORY_NAME, O_RDWR, S_IRUSR|S_IWUSR);

    /* Se não der erro no ftruncate define o tamanho da área de memória */
    ftruncate_with_validations(fd, data_size);
	
	/* Pointer para a memória partilhada */
    shared_data_type *shared_data = (shared_data_type*)mmap_with_validations(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    sem_t *sem[NUMBER_OF_SEMAPHORES];
    char semaphores_name[NUMBER_OF_SEMAPHORES][MAX_LENGTH];

    /* Abre os semáforos */
    for (i = 0; i < NUMBER_OF_SEMAPHORES; i++) {
        sprintf(semaphores_name[i], SEMAPHORE_NAME, i);
        sem[i] = sem_open(semaphores_name[i], O_CREAT);
        if (sem[i] == SEM_FAILED) {
            perror("An error occurred while trying to open a semaphore!\n");
            exit(EXIT_FAILURE);
        }
	}
    
    printf("*** Customer on hold ***\n");
    
    /* Espera até que o semáforo do cliente tenha um valor maior que zero, para então decrementá-lo */
    sem_wait_with_validation(sem[SEMAPHORE_INDEX_CLIENT]);

    printf("\n*** Customer to be served ***\n");

    /* Dorme um tempo aleatório para simular que está a ser atendido */
    random = generate_random_value(INITIAL_LIMIT, FINAL_LIMIT);
    sleep(random);

    printf("Ticket number %d!\n", shared_data->ticket_number);
    
    /* Incrementa e liberta o semáforo do vendedor */
    sem_post_with_validation(sem[SEMAPHORE_INDEX_SELLER]);   

    /* Se não der erro desconecta o pointer da memória partilhada */
    munmap_with_validations(shared_data, data_size);

    /* Se não der erro fecha a zona de memória partilhada */
    close_with_validations(fd);

    /* Se não der erro fecha os semáforos */
    for (i = 0; i < NUMBER_OF_SEMAPHORES; i++) {
        semaphore_close_with_validation(sem[i]);
    }  

	return 0;
}