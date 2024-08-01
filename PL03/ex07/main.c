#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include "shared_data_type.h"

#define ERROR_VALUE -1
#define NUMBER_CHILDREN 2
#define SHARED_MEMORY_NAME "/ex07"

int main() {
	int data_size = sizeof(struct aluno);
    pid_t p[NUMBER_CHILDREN];
    int min = INT_MAX, max = INT_MIN, total = 0, note, status;

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
	struct aluno *shared_data = (struct aluno*)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_data == MAP_FAILED) {
        perror("An error occurred while trying to map a shared memory object in the process address space!\n");
        exit(EXIT_FAILURE);
	}

    shared_data->dados_gravados = 0;

    for (int i = 0; i < NUMBER_CHILDREN; i++) {
        p[i] = fork();

        /* Caso aconteça uma falha na criação do processo filho */
		if (p[i] == ERROR_VALUE) {
			perror("Fork failed!\n");
			exit(EXIT_FAILURE);
		}
        
        /* Filho 1*/
        if (p[i] == 0) {

            /* Aguarda os dados do aluno */
            while (!shared_data->dados_gravados);

            switch(i) {
                /* Filho 1 */
                case 0:
                    /* Calcula a nota mais alta e a mais baixa */
                    for (int j = 0; j < NR_DISC; j++) {
                        note = shared_data->disciplinas[j];
                        
                        if (note > max) {
                            max = note;
                        }

                        if (min > note) {
                            min = note;
                        }
                    }

                    printf("Lowest grade: %d\n", min);
                    printf("Highest grade: %d\n", max);
                    break;

                /* Filho 2 */
                case 1:
                    /* Calcula a nota média */
                    for (int j = 0; j < NR_DISC; j++) {
                        note = shared_data->disciplinas[j];
                        
                        total += note;
                    }

                    printf("Average grade: %.2f\n", (float) total / NR_DISC);
                    break;
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

            exit(EXIT_SUCCESS);

        } else if (p[i] > 0 && i == 0) {
            printf("Insert the student number: \n");
            scanf("%d", &shared_data->numero);

            printf("Insert the student name: \n");
            scanf("%s", shared_data->nome);

            for (int j = 0; j < NR_DISC; j++) {
                printf("Insert the course grade %d: \n", j + 1);
                scanf("%d", &shared_data->disciplinas[j]);
            }

            shared_data->dados_gravados = 1;
        }
    }
    
    /* Esperar que os processos filhos terminem */
    for (int i = 0; i < NUMBER_CHILDREN; i++) {
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

	/* Se não der erro remove a zona de memória partilhada */
	if (shm_unlink(SHARED_MEMORY_NAME) == ERROR_VALUE) {
		perror("An error occurred while trying to remove memory area from file system!\n");
        exit(EXIT_FAILURE);
    }

	return 0;
}