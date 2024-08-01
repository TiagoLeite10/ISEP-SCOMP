#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <string.h>

#include "count_word_in_file.h"

#define ERROR_VALUE -1
#define SHARED_MEMORY_NAME "/ex08"
#define NUMBER_OF_PROCESSES 10

typedef struct {
	char path_to_file[19];
	char word_to_search[6];
	int num_occurrences;
	int can_start_search;
} child_process_information;

int main() {
	
	int fd;

	// Tamanho da área a partilhar
	int data_size = sizeof(child_process_information) * NUMBER_OF_PROCESSES;
	child_process_information *shared_data;

    /* Cria e abre a zona de memória partilhada */
	fd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd == ERROR_VALUE) {
		perror("An error occurred while trying to create and open a shared memory area!\n");
		return 1;
	}

	/* Define o tamanho da área da memória partilhada e inicializa-a a 0 */
	int truncate = ftruncate(fd, data_size);
	if (truncate == ERROR_VALUE) {
		perror("An error occurred while trying to define the size of the area!\n");
		return 1;
	}

    /* Pointer para a memória partilhada */
	shared_data = (child_process_information*) mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (shared_data == MAP_FAILED) {
		perror("An error occurred while trying to map a shared memory object in the process address space!\n");
		return 1;
	}

	// Criar os processos filhos
	pid_t p;
	int number_of_children;
	for (number_of_children = 0; number_of_children < NUMBER_OF_PROCESSES; number_of_children++) {
		p = fork();
		if (p == ERROR_VALUE) {
			perror("Fork failed!\n");
			return 1;
		}

		// Se for o processo filho
		if (p == 0)
			break;
	}

	// Se for o processo pai
	if (p > 0) {
		
		child_process_information* shared_data_temp_ptr = shared_data;

		// Preencher as estruturas com a informação necessária para cada processo
		for (int i = 0; i < NUMBER_OF_PROCESSES; i++) {
			strcpy(shared_data_temp_ptr->path_to_file, "./files/file");
			char file_num[3];
			sprintf(file_num, "%d", i + 1);
			strcat(shared_data_temp_ptr->path_to_file, file_num);
			strcat(shared_data_temp_ptr->path_to_file, ".txt");

			strcpy(shared_data_temp_ptr->word_to_search, "SCOMP");

			shared_data_temp_ptr->can_start_search = 1;

			shared_data_temp_ptr++;
		}

		// Esperar que os processos filhos terminem
		for (int i = 0; i < NUMBER_OF_PROCESSES; i++) {

			int status;
			if (wait(&status) == ERROR_VALUE) {
				perror("Something went wrong while waiting for my child process child to terminate!\n");
				return 1;
			}

			/* Se o processo filho não terminar com sucesso*/
			if (!WIFEXITED(status)) {
				perror("The child process did not finish successfully!\n");
				exit(EXIT_FAILURE);
			}
		}

		shared_data_temp_ptr = shared_data;
		// Imprimir o número de ocorrências determinadas por cada filho
		for (int i = 0; i < NUMBER_OF_PROCESSES; i++) {
			printf("Caminho para o ficheiro: %s\n", shared_data_temp_ptr->path_to_file);
			printf("Palavra a procurar: %s\n", shared_data_temp_ptr->word_to_search);
			printf("Número de ocorrências dessa palavra: %d\n", shared_data_temp_ptr->num_occurrences);
			printf("----- -----\n");
			shared_data_temp_ptr++;
		}
		
		// O processo pai pede para remover do sistema a área da memória partilhada
		if (shm_unlink(SHARED_MEMORY_NAME)) {
			perror("An error occurred while trying to remove memory area from file system!\n");
			return 1;
		}
		
	} else { // Se for o processo filho
	
		child_process_information* shared_data_temp_ptr = shared_data + number_of_children;
		
		// Espera ativa para o processo filho apenas realizar a sua tarefa após toda a informação estar-lhe disponível
		while (!shared_data_temp_ptr->can_start_search);

		// Contar quantas vezes uma dada palavra aparece no ficheiro
		int word_count = count_word_in_file(shared_data_temp_ptr->path_to_file, shared_data_temp_ptr->word_to_search);
		
		// Registar o número de ocorrências dessa palavra no respetivo ficheiro
		shared_data_temp_ptr->num_occurrences = word_count;
		
	}

    /* Se não der erro desconecta o pointer da memória partilhada */
	if (munmap(shared_data, data_size) == ERROR_VALUE) {
		perror("An error occurred while trying to disconnect the shared memory area from the process address space!\n");
		return 1;
	}

	/* Fecha o file descriptor */
	if (close(fd) == ERROR_VALUE) {
		perror("An error occurred while trying to close the file descriptor!\n");
		return 1;
	}

	// Terminar o processo filho com valor de sucesso
	if (p == 0) {
		exit(EXIT_SUCCESS);
	}

	return 0;

}
