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
#include "shared_data_type.h"

#define ERROR_VALUE -1
#define SHARED_MEMORY_NAME "/ex11"
#define SEMAPHORE_NAME "/sem_ex11_%d"
#define NUMBER_OF_SEMAPHORES 2
#define MAX_LENGTH 20
#define NUM_SELLERS 1
#define NUM_CUSTOMERS 5

void execute_program(char *program) {
    pid_t p = fork();
    
    if (p == 0) {
        execlp(program, program, (char *)NULL);
        exit(EXIT_SUCCESS);
    }
}

int main() {
    int data_size = sizeof(shared_data_type);
    int i, status;

    /* Cria e abre a zona de memória partilhada */
    int fd = shm_open_with_validations(SHARED_MEMORY_NAME, O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);

    /* Se não der erro no ftruncate define o tamanho da área da memória partilhada e inicializa-a a 0 */
	ftruncate_with_validations(fd, data_size);

    /* Pointer para a memória partilhada */
    shared_data_type *shared_data = (shared_data_type*)mmap_with_validations(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    sem_t *sem[NUMBER_OF_SEMAPHORES];
    char semaphores_name[NUMBER_OF_SEMAPHORES][MAX_LENGTH];

    /* Cria os semáforos */
    for (i = 0; i < NUMBER_OF_SEMAPHORES; i++) {
        sprintf(semaphores_name[i], SEMAPHORE_NAME, i);
        sem[i] = semaphore_open_with_validation(semaphores_name[i], O_CREAT | O_EXCL, 0644, 0);
	}

    /* Cria um processo filho para executar o vendedor */
    execute_program("./seller");

    /* Cria processos filhos para executar os clientes */
    for (i = 0; i < (NUM_CUSTOMERS); i++) {
        execute_program("./customer");
	}

    /* Esperar que os processos filhos terminem */
    for (i = 0; i < (NUM_SELLERS + NUM_CUSTOMERS); i++) {
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