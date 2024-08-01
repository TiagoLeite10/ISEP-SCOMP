#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <sys/mman.h>

#include "../../libs/my_shared_memory_lib.h"

#define SHARED_MEMORY_NAME "/ex02_shared_memory"
#define STRING_LENTH 80
#define NUMBER_OF_STRINGS 50

int main() {

    int fd;

    off_t data_size = sizeof(char) * STRING_LENTH * NUMBER_OF_STRINGS;

    char *shared_data;

    // Abrir a zona de memória partilhada no modo de apenas leitura
    fd = shm_open_with_validations(SHARED_MEMORY_NAME, O_RDONLY, S_IRUSR|S_IWUSR);

    // Pointer para a memória partilhada
    shared_data = (char *) mmap_with_validations(NULL, data_size, PROT_READ, MAP_SHARED, fd, 0);

    char *temp_shared_data;
    temp_shared_data = shared_data;

    int num_of_lines = 0;
    while (num_of_lines < NUMBER_OF_STRINGS && *temp_shared_data != '\0') {
        printf("%s\n", temp_shared_data); // Imprimir a string presente na memória partilhada no local apontado pelo apontador
        temp_shared_data += STRING_LENTH; // Avançar para o início da próxima string
        num_of_lines++;
    }

    printf("Total number of lines: %d\n", num_of_lines);

    munmap_with_validations(shared_data, data_size);
    close_with_validations(fd);

}
