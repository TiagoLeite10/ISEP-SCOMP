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

#include "libs/my_shared_memory_lib.h"
#include "libs/my_semaphore_lib.h"
#include "shared_data_type.h"

#define ERROR_VALUE -1
#define SHARED_MEMORY_NAME "/scomp"
#define SEMAPHORE_NAME "/scomp_%d"
#define SEMAPHORE_INDEX_MUTEX 0
#define SEMAPHORE_INDEX_FULL 1
#define SEMAPHORE_INDEX_EMPTY 2
#define NUMBER_OF_SEMAPHORES 3
#define NUMBER_OF_EXECUTIONS 100 /* Este número de execuções é apenas para o produtor/consumidor não estarem num ciclo infinito */
#define MAX_LENGTH 20

int main() {
    int data_size = sizeof(shared_data_type);
    int i, j, status;
    pid_t p;

    /* Cria e abre a zona de memória partilhada */
    int fd = shm_open_with_validations(SHARED_MEMORY_NAME, O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);

    /* Se não der erro no ftruncate define o tamanho da área da memória partilhada e inicializa-a a 0 */
	ftruncate_with_validations(fd, data_size);

    /* Pointer para a memória partilhada */
    shared_data_type *shared_data = (shared_data_type*)mmap_with_validations(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    sem_t *sem[NUMBER_OF_SEMAPHORES];
    char semaphores_name[NUMBER_OF_SEMAPHORES][MAX_LENGTH];
    int initial_values[NUMBER_OF_SEMAPHORES] = {1, 0, BUFFER_SIZE};

    /* Cria os semáforos */
    for (i = 0; i < NUMBER_OF_SEMAPHORES; i++) {
        sprintf(semaphores_name[i], SEMAPHORE_NAME, i);
        sem[i] = semaphore_open_with_validation(semaphores_name[i], O_CREAT | O_EXCL, 0644, initial_values[i]);
	}

    /* Inicializa o gerador de números aleatórios para os ids das boards */
    srand((unsigned) time(NULL));

    /* Cria um processo filho que representa o consumidor */
    p = fork();

    /* Caso aconteça uma falha na criação do processo filho */
    if (p == ERROR_VALUE) {
        perror("Fork failed!\n");
        exit(EXIT_FAILURE);
    }

    if (p == 0) {
        int read_index = 0;
        
        for (j = 0; j < NUMBER_OF_EXECUTIONS; j++) {
            /* Espera passivamente para que exista um item a ser consumido */
            sem_wait_with_validation(sem[SEMAPHORE_INDEX_FULL]);
            /* Esperar passivamente para realizar a exclusão mútua e ganhar acesso exclusivo à zona crítica */
            sem_wait_with_validation(sem[SEMAPHORE_INDEX_MUTEX]);

            /* Consume dados */
            printf("[%d] Consumer accessed the board with the id %d\n", getpid(), shared_data->data[read_index]);

            read_index++;
            /* Se o índice a ler já for fora do limite do buffer, voltar ao início do buffer */
            if (read_index == BUFFER_SIZE) {
                read_index = 0;
            }

            /* Libertar o acesso exclusivo à zona crítica */
            sem_post_with_validation(sem[SEMAPHORE_INDEX_MUTEX]);
            /* Informa que um valor foi consumido do buffer e que existe mais um espaço vazio */
            sem_post_with_validation(sem[SEMAPHORE_INDEX_EMPTY]);
        }                    

        /* Se não der erro desconecta o pointer da memória partilhada */
        munmap_with_validations(shared_data, data_size);

        /* Se não der erro fecha a zona de memória partilhada */
        close_with_validations(fd);

        /* Se não der erro fecha e remove os semáforos */
        for (i = 0; i < NUMBER_OF_SEMAPHORES; i++) {
            semaphore_close_with_validation(sem[i]);
        }            

        exit(EXIT_SUCCESS);
    }    

    /* Processo pai que representa o produtor*/
    for (j = 0; j < NUMBER_OF_EXECUTIONS; j++) {                
        /* Espera passivamente que exista espaço livre para escrever no buffer */
        sem_wait_with_validation(sem[SEMAPHORE_INDEX_EMPTY]);
        /* Espera passivamente para realizar a exclusão mútua e ganhar acesso exclusivo à zona crítica */
        sem_wait_with_validation(sem[SEMAPHORE_INDEX_MUTEX]);

        /* Se o local a escrever já for maior do que o tamanho do buffer, significa que temos de voltar à posição inicial do buffer */
        if (shared_data->write_index == BUFFER_SIZE) {
            shared_data->write_index = 0;
        }

        /* Produz dados */
        shared_data->data[shared_data->write_index] = rand() % 10000000;
        printf("[%d] Producer created the board with the id %d\n", getpid(), shared_data->data[shared_data->write_index]);

        /* Próxima posição do buffer onde será escrito o próximo valor */
        shared_data->write_index++;

        /* Libertar o acesso exclusivo à zona crítica */
        sem_post_with_validation(sem[SEMAPHORE_INDEX_MUTEX]);
        /* Informar que existe mais 1 item novo a ser lido do buffer */
        sem_post_with_validation(sem[SEMAPHORE_INDEX_FULL]);
    }

    /* Espera que o processo filho termine */
    if (wait(&status) == ERROR_VALUE) {
        perror("Something went wrong while waiting for my child process child to terminate!\n");
        exit(EXIT_FAILURE);
    }

    /* Se o processo filho não terminar com sucesso */
    if (!WIFEXITED(status)) {
        perror("The child process did not finish successfully!\n");
        exit(EXIT_FAILURE);
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