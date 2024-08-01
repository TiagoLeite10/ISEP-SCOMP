#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>

#define ERROR_VALUE -1

int shm_open_with_validations(const char *shm_name, int oflag, mode_t mode) {
    /* Cria e abre a zona de memória partilhada */
	int fd = shm_open(shm_name, oflag, mode);
	if (fd == ERROR_VALUE) {
		perror("An error occurred while trying to create and open a shared memory area, or open an existing one!\n");
		exit(EXIT_FAILURE);
	}

    return fd;
}

void ftruncate_with_validations(int fd, off_t data_size) {
    /* Se não der erro no ftruncate define o tamanho da área da memória partilhada e inicializa-a a 0 */
	if (ftruncate(fd, data_size) == ERROR_VALUE) {
		perror("An error occurred while trying to define the size of the area!\n");
		exit(EXIT_FAILURE);
	}
}

void *mmap_with_validations(void *addr, size_t data_size, int prot, int flags, int fd, off_t offset) {
    /* Pointer para a memória partilhada */
	void *shared_data = mmap(addr, data_size, prot, flags, fd, offset);
    if (shared_data == MAP_FAILED) {
        perror("An error occurred while trying to map a shared memory object in the process address space!\n");
        exit(EXIT_FAILURE);
	}

    return shared_data;
}

void shm_unlink_with_validations(char *shm_name) {
    /* Se não der erro remove a zona de memória partilhada */
    if (shm_unlink(shm_name) == ERROR_VALUE) {
        perror("An error occurred while trying to remove memory area from file system!\n");
        exit(EXIT_FAILURE);
    }
}

void munmap_with_validations(void *shared_data, size_t data_size) {
    /* Se não der erro desconecta o pointer da memória partilhada */
	if (munmap(shared_data, data_size) == ERROR_VALUE) {
		perror("An error occurred while trying to disconnect the shared memory area from the process address space!\n");
        exit(EXIT_FAILURE);
    }
}

void close_with_validations(int fd) {
    /* Se não der erro fecha a zona de memória partilhada */
	if (close(fd) == ERROR_VALUE) {
		perror("An error occurred while trying to close the file descriptor!\n");
        exit(EXIT_FAILURE);
    }
}