#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include "generate_random_value.h"
#include "shared_data_type.h"

#define ERROR_VALUE -1
#define SHARED_MEMORY_NAME "/ex03"
#define INITIAL_LIMIT 1
#define FINAL_LIMIT 20

int main() {
	int data_size = sizeof(shared_data_type);

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

    /* Preenche o array com números aleatório entre 1 e 20 */
    for (int i = 0; i < ARRAY_SIZE; i++) {
        shared_data->numbers[i] = generate_random_value(INITIAL_LIMIT, FINAL_LIMIT);
    }

	shared_data->can_read = 1;
    
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

	return 0;
}