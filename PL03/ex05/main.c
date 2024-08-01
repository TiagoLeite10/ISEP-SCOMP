#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include "shared_data_type.h"

#define ERROR_VALUE -1
#define SHARED_MEMORY_NAME "/ex05"
#define OPERATIONS 1000000
#define INITIAL_VALUE 100

volatile sig_atomic_t can_modify_father = 1;
volatile sig_atomic_t can_modify_children = 0;

void handle_signal_father(int signo, siginfo_t *sinfo, void *context) {
	if (signo == SIGUSR1) {
		can_modify_father = 1;
        can_modify_children = 0;
	}
}

void handle_signal_children(int signo, siginfo_t *sinfo, void *context) {
	if (signo == SIGUSR1) {
        can_modify_father = 0;
		can_modify_children = 1;
	}
}

int main() {
	int data_size = sizeof(shared_data_type);
    int i = 0;
    pid_t p;
    struct timespec ts;

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

    shared_data->value = INITIAL_VALUE;
    
    p = fork();

    /* Caso aconteça uma falha na criação do processo filho */
    if (p == ERROR_VALUE) {
        perror("Fork failed!\n");
        exit(EXIT_FAILURE);
    }

    if (p > 0) {
        /* Configura o tratamento de sinais do pai */
        struct sigaction act;
        memset(&act, 0, sizeof(struct sigaction));
        sigemptyset(&act.sa_mask);
        act.sa_sigaction = handle_signal_father;
        act.sa_flags = SA_SIGINFO;
        sigaction(SIGUSR1, &act, NULL);
        
        for (i = 0; i < OPERATIONS; i++) {
            while(!can_modify_father) {
                pause();
            }

            shared_data->value++;
            shared_data->value--;

            /* Envia o sinal para o filho */
            kill(p, SIGUSR1);
                        
            /* Aguarda um tempo aleatório entre 0 e 1000 microssegundos */
            ts.tv_sec = 0;
            ts.tv_nsec = rand() % 1000 * 1000;
            nanosleep(&ts, NULL);
        }
        
        /* Aguarda que o processo filho termine */
        if (wait(NULL) == ERROR_VALUE) {
            perror("Something went wrong while waiting for my child process child to terminate!\n");
            exit(EXIT_FAILURE);
        }
        
        printf("Final value: %d\n", shared_data->value);

		/* O processo pai pede para remover do sistema a área da memória partilhada */
		if (shm_unlink(SHARED_MEMORY_NAME) == ERROR_VALUE) {
			perror("An error occurred while trying to remove memory area from file system!\n");
			exit(EXIT_FAILURE);
		}
        
    } else if (p == 0) {
        /* Configura o tratamento de sinais do filho */
        struct sigaction act;
        memset(&act, 0, sizeof(struct sigaction));
        sigemptyset(&act.sa_mask);
        act.sa_sigaction = handle_signal_children;
        act.sa_flags = SA_SIGINFO;
        sigaction(SIGUSR1, &act, NULL);

        for (i = 0; i < OPERATIONS; i++) {
            while(!can_modify_children) {
                pause();
            }
            
            shared_data->value++;
            shared_data->value--;

            /* Envia o sinal para o pai */
            kill(getppid(), SIGUSR1);

            /* Aguarda um tempo aleatório entre 0 e 1000 microssegundos */
            ts.tv_sec = 0;
            ts.tv_nsec = rand() % 1000 * 1000;
            nanosleep(&ts, NULL);
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