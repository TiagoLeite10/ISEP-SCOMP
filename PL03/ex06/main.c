#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>

#include "generate_random_values.h"

#define ERROR_VALUE -1
#define SHARED_MEMORY_NAME "/ex06"
#define NUM_INTEGERS_IN_ARRAY 1000
#define RANDOM_NUMBER_LIMIT 1000
#define NUMBER_OF_PROCESSES 10

int main() {
	
	// Array com NUM_INTEGERS_IN_ARRAY valores random
	int random_array[NUM_INTEGERS_IN_ARRAY] = {0};
	generate_random_values(random_array, NUM_INTEGERS_IN_ARRAY, RANDOM_NUMBER_LIMIT);

	// Número de valores a serem tratados por cada processo
	int size_per_process = NUM_INTEGERS_IN_ARRAY / NUMBER_OF_PROCESSES;

	int fd;

	// Número de bytes ocupados por um array de inteiros com NUMBER_OF_PROCESSES posições (tamanho)
	int data_size = sizeof(int) * NUMBER_OF_PROCESSES;
	int *shared_data;

    /* Cria e abre a zona de memória partilhada */
	fd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
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
	shared_data = (int*) mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (shared_data == MAP_FAILED) {
		perror("An error occurred while trying to map a shared memory object in the process address space!\n");
		exit(EXIT_FAILURE);
	}

	// Criar os processos filhos
	pid_t p;
	int number_of_children;
	for (number_of_children = 0; number_of_children < NUMBER_OF_PROCESSES; number_of_children++) {
		p = fork();
		if (p == ERROR_VALUE) {
			perror("Fork failed!\n");
			exit(EXIT_FAILURE);
		}

		// Se for o processo filho
		if (p == 0)
			break;
	}

	// Se for o processo pai
	if (p > 0) {
		// Espera que todos os processos filhos terminem de procurar o número máximo local
		for (int i = 0; i < NUMBER_OF_PROCESSES; i++) {

			int status;
			// Verificar se a espera pelos processos filhos funciona corretamente
			if (wait(&status) == ERROR_VALUE) {
				perror("Something went wrong while waiting for my child process child to terminate!\n");
				exit(EXIT_FAILURE);
			}

			/* Se o processo filho não terminar com sucesso*/
			if (!WIFEXITED(status)) {
				perror("The child process did not finish successfully!\n");
				exit(EXIT_FAILURE);
			}
		}

		// Verifica qual é o valor máximo do array de número random
		int globalMaximum = 0;
		for (int i = 0; i < NUMBER_OF_PROCESSES; i++) {
			int actual_value = *(shared_data + i);
			if (globalMaximum < actual_value) {
				globalMaximum = actual_value;
			}
		}

		// Imprime o valor máximo de cada range do array
		for (int i = 0; i < NUMBER_OF_PROCESSES; i++) {
			int range_start = size_per_process * i;
			printf("Range do array: [%d ; %d].\n", range_start, range_start + size_per_process - 1);
			printf("O valor máximo existente neste intervalo é: %d!\n", *(shared_data + i));
			printf("----- -----\n");
		}

		printf("O maior valor do array de números random é: %d!\n", globalMaximum);
		
		// O processo pai pede para remover do sistema a área da memória partilhada
		if (shm_unlink(SHARED_MEMORY_NAME)) {
			perror("An error occurred while trying to remove memory area from file system!\n");
			exit(EXIT_FAILURE);
		}
		
	} else { // Se for o processo filho

		// Array que vai apontar para o valor onde a procura deve começar
		int *temp_arr;
		int start_offset = number_of_children * size_per_process;
		temp_arr = random_array + start_offset;

		int largest_element = 0;
		// Encontra o maior valor em size_per_process posições do array com valores random, a começar na posição definida para este processo
		for (int i = 0; i < size_per_process; i++) {
			int actual_value = *(temp_arr + i);
			if (largest_element < actual_value)
				largest_element = actual_value;
		}

		// Escreve o maior valor local na posição correspondente do array partilhado
		*(shared_data + number_of_children) = largest_element;
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
