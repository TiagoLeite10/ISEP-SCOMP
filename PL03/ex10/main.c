#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "generate_random_value.h"

#define ERROR_VALUE -1
#define SHARED_MEMORY_NAME "/ex09"
#define INITIAL_LIMIT 1
#define FINAL_LIMIT 20
#define TOTAL_DATA 30
#define ARRAY_SIZE 10
#define READ 0
#define WRITE 1

int main() {

    int fd_sync_producer_access[2];
    int fd_sync_consumer_access[2];

    int * shared_data_temp_ptr;

	int data_size = sizeof(int) * ARRAY_SIZE;
    
    int i = 0, j = 0, k = 0;
    pid_t p;

    char value_one_char = 1;
    char temp_read;

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
	int *shared_data = (int*) mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_data == MAP_FAILED) {
        perror("An error occurred while trying to map a shared memory object in the process address space!\n");
        exit(EXIT_FAILURE);
	}

    // Cria o pipe
    if (pipe(fd_sync_producer_access) == ERROR_VALUE || pipe(fd_sync_consumer_access) == ERROR_VALUE) {
        perror("Pipe failed!\n");
        exit(EXIT_FAILURE);
    }

    // Coloca 10 chars no pipe que controla se o processo produtor pode produzir mais valores
    for (int i = 0; i < ARRAY_SIZE; i++) {
        write(fd_sync_producer_access[WRITE], &value_one_char, sizeof(char));
    }

    p = fork();
    /* Caso aconteça uma falha na criação do processo filho */
    if (p == ERROR_VALUE) {
        perror("Fork failed!\n");
        exit(EXIT_FAILURE);
    }

    if (p > 0) {

        // Fecha a leitura no pipe que envia informação para o processo filho, e fecha a escrita no pipe que recebe informação do processo filho
        if (close(fd_sync_consumer_access[READ]) == ERROR_VALUE || close(fd_sync_producer_access[WRITE]) == ERROR_VALUE) {
            perror("An error occurred while trying to close the pipe!\n");
            exit(EXIT_FAILURE);
        }

        while (i < TOTAL_DATA) {
            
            // Voltar a apontar para a posição inicial do array
            shared_data_temp_ptr = shared_data;

            for (j = 0; j < ARRAY_SIZE; j++) {
                // Verifica se pode gerar mais valores ou se tem de esperar
                if (read(fd_sync_producer_access[READ], &temp_read, sizeof(temp_read)) == ERROR_VALUE) {
                    printf("An error occurred while trying to read from producer sync pipe!\n");
                    exit(EXIT_FAILURE);
                }
                
                *shared_data_temp_ptr = generate_random_value(INITIAL_LIMIT, FINAL_LIMIT);

                printf("Producer[%d] -> %d\n", j, *shared_data_temp_ptr);
                
                // Informa que existe um valor para ler
                if (write(fd_sync_consumer_access[WRITE], &value_one_char, sizeof(value_one_char)) == ERROR_VALUE) {
                    printf("An error occurred while trying to write in the consumer sync pipe!\n");
                    exit(EXIT_FAILURE);
                }

                shared_data_temp_ptr++;
            }

            i += j;
        }

        // Fecha a escrita no pipe que envia informação para o processo filho
        if (close(fd_sync_consumer_access[WRITE]) == ERROR_VALUE) {
            perror("An error occurred while trying to close the pipe!\n");
            exit(EXIT_FAILURE);
        }

        int status;
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

        // Fecha o pipe que lê informações provenientes do processo filho
        if (close(fd_sync_producer_access[READ]) == ERROR_VALUE) {
            perror("An error occurred while trying to close the pipe!\n");
            exit(EXIT_FAILURE);
        }

        /* Se não der erro remove a zona de memória partilhada */
        if (shm_unlink(SHARED_MEMORY_NAME)) {
            perror("An error occurred while trying to remove memory area from file system!\n");
            exit(EXIT_FAILURE);
        }

    } else if (p == 0) {

        // Fecha a leitura no pipe que envia informação para o processo pai, e fecha a escrita no pipe que recebe informação do processo pai
        if (close(fd_sync_consumer_access[WRITE]) == ERROR_VALUE || close(fd_sync_producer_access[READ]) == ERROR_VALUE) {
            perror("An error occurred while trying to close the pipe!\n");
            exit(EXIT_FAILURE);
        }
        
        while (i < TOTAL_DATA) {

            // Voltar a apontar para a posição inicial do array
            shared_data_temp_ptr = shared_data;

            for (j = 0; j < ARRAY_SIZE; j++) {
                // Verifica se existe valores para ler
                if (read(fd_sync_consumer_access[READ], &temp_read, sizeof(temp_read)) == ERROR_VALUE) {
                    printf("An error occurred while trying to read from consumer sync pipe!\n");
                    exit(EXIT_FAILURE);
                }

                printf("Consumer[%d] -> %d\n", k, *shared_data_temp_ptr);

                // Informa que um valor foi lido da memória partilhada
                if (write(fd_sync_producer_access[WRITE], &value_one_char, sizeof(value_one_char)) == ERROR_VALUE) {
                    printf("An error occurred while trying to write in the consumer sync pipe!\n");
                    exit(EXIT_FAILURE);
                }

                shared_data_temp_ptr++;
                k++;
            }
            
            i += j;
        }

        // Fecha neste processo filho os pipes em uso
        if (close(fd_sync_consumer_access[READ]) == ERROR_VALUE || close(fd_sync_producer_access[WRITE]) == ERROR_VALUE) {
            perror("An error occurred while trying to close the pipe!\n");
            exit(EXIT_FAILURE);
        }

    }

    /* Se não der erro desconecta o pointer da memória partilhada */
	if (munmap(shared_data, data_size) == ERROR_VALUE) {
		perror("An error occurred while trying to disconnect the shared memory area from the process address space!\n");
        exit(EXIT_FAILURE);
    }

    /* Se não der erro fecha a zona de memória partilhada */
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
