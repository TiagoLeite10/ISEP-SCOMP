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
#include "generate_random_values.h"
#include "shared_data_type.h"

#define ERROR_VALUE -1
#define SHARED_MEMORY_NAME "/ex14"
#define SEMAPHORE_NAME "/sem_ex14_%d"
#define SEMAPHORE_INDEX_EXCL_A 0
#define SEMAPHORE_INDEX_EXCL_B 1
#define SEMAPHORE_INDEX_NO_USE 2
#define SEMAPHORE_INDEX_EXCL 3
#define NUMBER_OF_PROCESSES_AX 3
#define NUMBER_OF_PROCESSES_BX 2
#define NUMBER_OF_SEMAPHORES 4
#define MAX_LENGTH 20
#define LIMIT_NUMBER 10

void print_vetor(int *numbers, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", numbers[i]);
    }
    printf("\n");
}

int main() {
    int data_size = sizeof(shared_data_type);
    int i, status;
    pid_t p;

    sem_t *sem[NUMBER_OF_SEMAPHORES];
    char semaphores_name[NUMBER_OF_SEMAPHORES][MAX_LENGTH];
	int initial_values[NUMBER_OF_SEMAPHORES] = {1, 1, 1, 1};

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

    /* Para os processos do tipo Ax */
    for (i = 0; i < NUMBER_OF_PROCESSES_AX; i++) {
        p = fork();

        /* Caso aconteça uma falha na criação do processo filho */
        if (p == ERROR_VALUE) {
            perror("Fork failed!\n");
            exit(EXIT_FAILURE);
        }

        if (p == 0) {           
            /* Aguarda o semáforo exclusivo para acesso aos dados compartilhados nos processo do tipo Ax */
            sem_wait_with_validation(sem[SEMAPHORE_INDEX_EXCL_A]);

            /* Incrementa o número de processos do tipo Ax */
            shared_data->num_readers_a++;

            /* Se é o primeiro processo Ax, aguarda o semáforo de exclusão mútua para garantir que nenhum processo Bx esteja a escrever */
            if (shared_data->num_readers_a == 1) {
                sem_wait_with_validation(sem[SEMAPHORE_INDEX_NO_USE]);
            }

            /* Liberta o semáforo exclusivo para acesso aos dados compartilhados nos processo do tipo Ax */
            sem_post_with_validation(sem[SEMAPHORE_INDEX_EXCL_A]);
            
            /* Aguarda o semáforo exclusivo para acesso aos dados compartilhados */
            sem_wait_with_validation(sem[SEMAPHORE_INDEX_EXCL]);

            /* Escreve e lê da memória partilhada */
            sleep(5);
            generate_random_values(shared_data->numbers, VETOR_SIZE, LIMIT_NUMBER);
            
            printf("Process Ax\n");
            print_vetor(shared_data->numbers, VETOR_SIZE);

            /* Liberta o semáforo exclusivo para acesso aos dados compartilhados */
            sem_post_with_validation(sem[SEMAPHORE_INDEX_EXCL]);

            /* Aguarda o semáforo exclusivo para acesso aos dados compartilhados nos processo do tipo Ax */
            sem_wait_with_validation(sem[SEMAPHORE_INDEX_EXCL_A]);
           
            /* Decrementa o número de processos do tipo Ax */
            shared_data->num_readers_a--;

            /* Se não há mais processos Ax, liberta o semáforo exclusivo para acesso a dados compartilhados nos processo do tipo Bx */
            if (shared_data->num_readers_a == 0) {
                sem_post_with_validation(sem[SEMAPHORE_INDEX_NO_USE]);
            }

            /* Liberta o semáforo exclusivo para acesso aos dados compartilhados nos processo do tipo Ax */
            sem_post_with_validation(sem[SEMAPHORE_INDEX_EXCL_A]);

            /* Se não der erro desconecta o pointer da memória partilhada */
            munmap_with_validations(shared_data, data_size);

            /* Se não der erro fecha a zona de memória partilhada */
            close_with_validations(fd);

            /* Se não der erro fecha o semáforo */
            for (i = 0; i < NUMBER_OF_SEMAPHORES; i++) {
                semaphore_close_with_validation(sem[i]);
            }

            exit(EXIT_SUCCESS);
        }
    }

    /* Para os processos do tipo Bx */
    for (i = 0; i < NUMBER_OF_PROCESSES_BX; i++) {
        p = fork();

        /* Caso aconteça uma falha na criação do processo filho */
        if (p == ERROR_VALUE) {
            perror("Fork failed!\n");
            exit(EXIT_FAILURE);
        }

        if (p == 0) {
            /* Aguarda o semáforo exclusivo para acesso aos dados compartilhados nos processo do tipo Bx */
            sem_wait_with_validation(sem[SEMAPHORE_INDEX_EXCL_B]);

            /* Incrementa o número de processos do tipo Bx */
            shared_data->num_readers_b++;

            /* Se é o primeiro processo Bx, aguarda o semáforo de exclusão mútua para garantir que nenhum processo Bx esteja a escrever */
            if (shared_data->num_readers_b == 1) {
                sem_wait_with_validation(sem[SEMAPHORE_INDEX_NO_USE]);
            }

            /* Liberta o semáforo exclusivo para acesso aos dados compartilhados nos processo do tipo Bx */
            sem_post_with_validation(sem[SEMAPHORE_INDEX_EXCL_B]);

            /* Aguarda o semáforo exclusivo para acesso aos dados compartilhados */
            sem_wait_with_validation(sem[SEMAPHORE_INDEX_EXCL]);

            /* Escreve e lê da memória partilhada */
            sleep(6);
            generate_random_values(shared_data->numbers, VETOR_SIZE, LIMIT_NUMBER);
            
            printf("Process Bx\n");
            print_vetor(shared_data->numbers, VETOR_SIZE);

            /* Liberta o semáforo exclusivo para acesso aos dados compartilhados */
            sem_post_with_validation(sem[SEMAPHORE_INDEX_EXCL]);

            /* Aguarda o semáforo exclusivo para acesso aos dados compartilhados nos processo do tipo Bx */
            sem_wait_with_validation(sem[SEMAPHORE_INDEX_EXCL_B]);
           
            /* Decrementa o número de processos do tipo Bx */
            shared_data->num_readers_b--;

            /* Se não há mais processos Bx, liberta o semáforo exclusivo para acesso a dados compartilhados nos processo do tipo Ax */
            if (shared_data->num_readers_b == 0) {
                sem_post_with_validation(sem[SEMAPHORE_INDEX_NO_USE]);
            }

            /* Liberta o semáforo exclusivo para acesso aos dados compartilhados nos processo do tipo Bx */
            sem_post_with_validation(sem[SEMAPHORE_INDEX_EXCL_B]);

            /* Se não der erro desconecta o pointer da memória partilhada */
            munmap_with_validations(shared_data, data_size);

            /* Se não der erro fecha a zona de memória partilhada */
            close_with_validations(fd);

            /* Se não der erro fecha o semáforo */
            for (i = 0; i < NUMBER_OF_SEMAPHORES; i++) {
                semaphore_close_with_validation(sem[i]);
            }

            exit(EXIT_SUCCESS);
        }
    }

    /* Esperar que os processos filhos terminem */
    for (i = 0; i < NUMBER_OF_PROCESSES_AX + NUMBER_OF_PROCESSES_BX; i++) {
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