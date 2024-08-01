#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define ERROR_VALUE -1
#define SHARED_MEMORY_NAME "/shm_ex02_with_data_consistency"
#define FIRST_INTEGER_START_VALUE 10000
#define SECOND_INTEGER_START_VALUE 500
#define NUM_OPERATIONS_PERFORM 1000000

typedef struct {
	int first_integer;
	int second_integer;
	int parent_is_working;
} shared_two_integers;

int main() {	
	
	int fd;
	int data_size = sizeof(shared_two_integers);
	shared_two_integers *shared_data;

    /* Cria e abre a zona de memória partilhada */
	fd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_EXCL | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd == ERROR_VALUE) {
		perror("An error occurred while trying to create and open a shared memory area!\n");
		exit(EXIT_FAILURE);
	}

	/* Define o tamanho da área da memória partilhada e inicializa-a a 0 */
	int truncate = ftruncate(fd, data_size);
	if (truncate == ERROR_VALUE) {
		perror("An error occurred while trying to define the size of the area!\n");
		exit(EXIT_FAILURE);
	}

    /* Pointer para a memória partilhada */
	shared_data = (shared_two_integers*) mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (shared_data == MAP_FAILED) {
		perror("An error occurred while trying to map a shared memory object in the process address space!\n");
		exit(EXIT_FAILURE);
	}

	shared_data->first_integer = FIRST_INTEGER_START_VALUE;
	shared_data->second_integer = SECOND_INTEGER_START_VALUE;
	shared_data->parent_is_working = 1;

	pid_t p = fork();
	if (p == ERROR_VALUE) {
		perror("Fork failed!\n");
		exit(EXIT_FAILURE);
	}

	// Se for o pai
	if (p > 0) {
		for (int i = 0; i < NUM_OPERATIONS_PERFORM; i++) {
			while (!shared_data->parent_is_working);
			shared_data->first_integer--;
			shared_data->second_integer++;
			shared_data->parent_is_working = 0;
		}

		int status;
		if (wait(&status) == ERROR_VALUE) {
			perror("Something went wrong while waiting for my child process child to terminate!\n");
			exit(EXIT_FAILURE);
		}

		/* Se o processo filho não terminar com sucesso*/
        if (!WIFEXITED(status)) {
            perror("The child process did not finish successfully!\n");
            exit(EXIT_FAILURE);
        }

		printf("First integer final value: %d\n", shared_data->first_integer);
		printf("Second integer final value: %d\n", shared_data->second_integer);

		// O processo pai pede para remover do sistema a área da memória partilhada
		if (shm_unlink(SHARED_MEMORY_NAME)) {
			perror("An error occurred while trying to remove memory area from file system!\n");
			exit(EXIT_FAILURE);
		}

	} else { // Se for o filho
		for (int i = 0; i < NUM_OPERATIONS_PERFORM; i++) {
			while (shared_data->parent_is_working);
			shared_data->first_integer++;
			shared_data->second_integer--;
			shared_data->parent_is_working = 1;
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
