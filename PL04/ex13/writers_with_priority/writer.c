#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <time.h>

#include "../../libs/my_shared_memory_lib.h"
#include "../../libs/my_semaphore_lib.h"
#include "shared_data_type.h"

#define ERROR_VALUE -1
#define SHARED_MEMORY_NAME "/ex13"
#define SEMAPHORE_NAME "/sem_ex13_%d"
#define SEMAPHORE_INDEX_EXCL 0
#define SEMAPHORE_INDEX_NO_USE 1
#define SEMAPHORE_INDEX_TORNIQUETE 2
#define NUMBER_OF_SEMAPHORES 3
#define MAX_LENGTH 20

int main() {
    int data_size = sizeof(shared_data_type);
    int i;
   
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

    /* Espera o semáforo para garantir que apenas um processo use a seção crítica por vez */
    sem_wait_with_validation(sem[SEMAPHORE_INDEX_TORNIQUETE]);

    /* Espera o semáforo para garantir que não há leitores na secção crítica */
    sem_wait_with_validation(sem[SEMAPHORE_INDEX_NO_USE]);
    
    /* Incrementa o número de escritores */
    shared_data->num_writers++;
    
    /* Obtêm a hora atual e armazena na região de memória compartilhada */
    time_t now = time(0);
    struct tm *time_info = localtime(&now);
    sprintf(shared_data->phrase, "[%d] %02d:%02d:%02d", getpid(), time_info->tm_hour, time_info->tm_min, time_info->tm_sec);

    /* Imprime o número de escritores e leitores naquele momento */
    printf("Writer -> Number of writers at that moment: %d\n", shared_data->num_writers);
    printf("Writer -> Number of readers at that moment: %d\n", shared_data->num_readers); 

    /* Decrementa o número de escritores */
    shared_data->num_writers--;

    /* Liberta o semáforo para permitir que outro processo use a seção crítica */
    sem_post_with_validation(sem[SEMAPHORE_INDEX_TORNIQUETE]);

    /* Liberta o semáforo para permitir que leitores acessem a seção crítica */
    sem_post_with_validation(sem[SEMAPHORE_INDEX_NO_USE]); 
    
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