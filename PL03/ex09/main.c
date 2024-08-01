#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "generate_random_value.h"
#include "shared_data_type.h"

#define ERROR_VALUE -1
#define SHARED_MEMORY_NAME "/ex09"
#define INITIAL_LIMIT 1
#define FINAL_LIMIT 20
#define TOTAL_DATA 30

int main() {
	int data_size = sizeof(shared_data_type);
    int i = 0, j = 0, k = 0, status;
    pid_t p;

    /* Cria e abre a zona de memória partilhada */
	int fd = shm_open(SHARED_MEMORY_NAME, O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
	if (fd == ERROR_VALUE) {
		perror("An error occurred while trying to create and open a shared memory area!\n");
		exit(EXIT_FAILURE);
	}

    /* Se não der erro no ftruncate define o tamanho da área da memória partilhada e inicializa-a a 0 */
	if (ftruncate(fd, data_size) == ERROR_VALUE) {
		perror("An error occurred while trying to define the size of the area!\n");
		exit(EXIT_FAILURE);
	}

    /* Pointer para a memória partilhada */
	shared_data_type *shared_data = (shared_data_type*)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_data == MAP_FAILED) {
        perror("An error occurred while trying to map a shared memory object in the process address space!\n");
        exit(EXIT_FAILURE);
	}

    shared_data->can_write = 1;
    shared_data->number_elements = 0;

    p = fork();

    /* Caso aconteça uma falha na criação do processo filho */
    if (p == ERROR_VALUE) {
        perror("Fork failed!\n");
        exit(EXIT_FAILURE);
    }

    if (p > 0) {

        while (i < TOTAL_DATA) {
            
            for (j = 0; j < ARRAY_SIZE; j++) {
                /* Se o número de elementos for igual ao tamanho do array não pode continuar a escrever */
                if (shared_data->number_elements == ARRAY_SIZE) {
                    shared_data->can_write = 0;
                }

                /* Enquanto não pode escrever, fica em espera ativa */
                while (!shared_data->can_write);
                
                shared_data->numbers[j] = generate_random_value(INITIAL_LIMIT, FINAL_LIMIT);
                shared_data->number_elements++;

                printf("Producer[%d] -> %d\n", j, shared_data->numbers[j]);
            }

            i+= j;
        }
        
        /* Aguarda que o processo filho termine */
        if (wait(&status) == ERROR_VALUE) {
            perror("Something went wrong while waiting for my child process child to terminate!\n");
            exit(EXIT_FAILURE);
        }

        /* Se o processo filho não terminar com sucesso*/
        if (!WIFEXITED(status)) {
            perror("The child process did not finish successfully!\n");
            exit(EXIT_FAILURE);
        }

		/* O processo pai pede para remover do sistema a área da memória partilhada */
		if (shm_unlink(SHARED_MEMORY_NAME) == ERROR_VALUE) {
			perror("An error occurred while trying to remove memory area from file system!\n");
			exit(EXIT_FAILURE);
		}

    } else if (p == 0) {

        while (i < TOTAL_DATA) {

            for (j = 0; j < ARRAY_SIZE; j++) {
                /* Enquanto não exitem elementos para ler, fica em espera ativa */
                while (!shared_data->number_elements);

                /* Se o número de elementos for menor que o tamanho do array pode continuar a escrever */
                if (shared_data->number_elements < ARRAY_SIZE) {
                    shared_data->can_write = 1;
                }

                printf("Consumer[%d] -> %d\n", k, shared_data->numbers[j]);
                shared_data->number_elements--;

                k++;
            }
            
            i+= j;
        }
    }

    /* Se não der erro desconecta o pointer da memória partilhada */
	if (munmap(shared_data, data_size) == ERROR_VALUE) {
		perror("An error occurred while trying to disconnect the shared memory area from the process address space!\n");
		exit(EXIT_FAILURE);
	}

	/* Fecha o file descriptor */
	if (close(fd) == ERROR_VALUE) {
		perror("An error occurred while trying to close the file descriptor!\n");
		exit(EXIT_FAILURE);
	}

    // Terminar o processo filho com valor de sucesso
	if (p == 0) {
		exit(EXIT_SUCCESS);
	}

	return 0;
}