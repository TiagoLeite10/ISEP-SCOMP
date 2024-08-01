#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <limits.h>

#include "../libs/my_shared_memory_lib.h"
#include "../libs/my_semaphore_lib.h"
#include "generate_random_values.h"
#include "shared_data_type.h"

#define ERROR_VALUE -1
#define SHARED_MEMORY_NAME "/ex16"
#define SEMAPHORE_NAME "/sem_ex16_%d"
#define SEMAPHORE_INDEX_1 0
#define SEMAPHORE_INDEX_2 1
#define NUMBER_OF_PROCESSES 5
#define NUMBER_OF_SEMAPHORES 2
#define MAX_LENGTH 20
#define NUMBERS_ANALYZE 10
#define LIMIT_NUMBER 10000

int main() {
    int data_size = sizeof(shared_data_type);
    int i, j, k, number, flag = 0, status;
    pid_t p[NUMBER_OF_PROCESSES], additional_p;

    sem_t *sem[NUMBER_OF_SEMAPHORES];
    char semaphores_name[NUMBER_OF_SEMAPHORES][MAX_LENGTH];
	int initial_values[NUMBER_OF_SEMAPHORES] = {1, 0};

    /* Cria e abre a zona de memória partilhada */
    int fd = shm_open_with_validations(SHARED_MEMORY_NAME, O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);

    /* Se não der erro no ftruncate define o tamanho da área da memória partilhada e inicializa-a a 0 */
    ftruncate_with_validations(fd, data_size);

    /* Pointer para a memória partilhada */
    shared_data_type *shared_data = (shared_data_type*)mmap_with_validations(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    
    /* Cria os semáforos */
    for (i = 0; i < NUMBER_OF_SEMAPHORES; i++) {
        sprintf(semaphores_name[i], SEMAPHORE_NAME, i);
        sem[i] = semaphore_open_with_validation(semaphores_name[i], O_CREAT | O_EXCL, 0644, initial_values[i]);
	}

    /* Gerar valores aleatórios para o vetor inicial */
    generate_random_values(shared_data->initial_vector, INITIAL_VETOR_SIZE, LIMIT_NUMBER);

    shared_data->max_number = INT_MIN;
    shared_data->next_position = 0;
    shared_data->flag = 0;

    int numbers_analyze_child = INITIAL_VETOR_SIZE / NUMBER_OF_PROCESSES;

    /* Cria os processos filhos em que cada um processa 1/5 do vetor inicial */
    for (i = 0; i < NUMBER_OF_PROCESSES; i++) {
        p[i] = fork();

        /* Caso aconteça uma falha na criação do processo filho */
        if (p[i] == ERROR_VALUE) {
            perror("Fork failed!\n");
            exit(EXIT_FAILURE);
        }

        if (p[i] == 0) {
            int initial_index = i * numbers_analyze_child;
            int final_index =  initial_index + numbers_analyze_child;

			for (j = initial_index; j < final_index; j+= NUMBERS_ANALYZE) {
                int total = 0;

                /* Aguarda o semáforo exclusivo para acesso à zona crítica do vetor inicial */
                sem_wait_with_validation(sem[SEMAPHORE_INDEX_1]);

                /* Realiza a soma móvel de 10 valores para ser calculada a média futuramente */
				for (k = j; k < j + NUMBERS_ANALYZE ; k++) {
                    total += shared_data->initial_vector[k];
                }

                /* Calculado a média móvel de 10 valores */
                shared_data->final_vector[shared_data->next_position] = total / NUMBERS_ANALYZE;                
                number = shared_data->final_vector[shared_data->next_position];                
                
                shared_data->next_position++;
                
                /* Verifica se o valor encontrado é maior que o atual maior valor do vetor final */
                if (number > shared_data->max_number) {
                    /* Atualiza o maior valor encontrado */
                    shared_data->max_number = number;      
                    /* Liberta o semáforo responsável por imprimir uma mensagem de aviso sempre que um novo valor maior for encontrado no vetor final */
                    sem_post_with_validation(sem[SEMAPHORE_INDEX_2]);
                } else {
                    /* Liberta o semáforo exclusivo para acesso à zona crítica do vetor inicial */
                    sem_post_with_validation(sem[SEMAPHORE_INDEX_1]);
                }        
            }

            /* Se não der erro desconecta o pointer da memória partilhada */
            munmap_with_validations(shared_data, data_size);

            /* Se não der erro fecha a zona de memória partilhada */
            close_with_validations(fd);

            /* Se não der erro fecha o semáforo */
            for (i = 0; i < NUMBER_OF_SEMAPHORES; i++) {
                semaphore_close_with_validation(sem[i]);
            }

            exit(EXIT_SUCCESS);
        }
    }

    /* Crie outro processo filho para encontrar o número máximo do vetor final */
    additional_p = fork();

    /* Caso aconteça uma falha na criação do processo filho */
    if (additional_p == ERROR_VALUE) {
        perror("Fork failed!\n");
        exit(EXIT_FAILURE);
    }

    /* Imprime uma mensagem de aviso sempre que um novo valor maior for encontrado no vetor final */
    if (additional_p == 0) {
        while(!flag) {
            /* Aguarda o semáforo responsável por imprimir uma mensagem de aviso sempre que um novo valor maior for encontrado no vetor final */
            sem_wait_with_validation(sem[SEMAPHORE_INDEX_2]);        

            printf("A new larger value was found in the final vector!\n");
            
            /* Atualiza o valor da flag para perceber se já todos os filhos analisaram o vetor inicial */
            flag = shared_data->flag; 

            /* Liberta o semáforo exclusivo para acesso à zona crítica do vetor inicial */
            if (!flag) {
                sem_post_with_validation(sem[SEMAPHORE_INDEX_1]);    
            }                
        }

        /* Se não der erro fecha o semáforo */
        for (i = 0; i < NUMBER_OF_SEMAPHORES; i++) {
            semaphore_close_with_validation(sem[i]);
        }

        exit(EXIT_SUCCESS);
    }
        
    /* Esperar que os processos filhos terminem */
    for (i = 0; i < NUMBER_OF_PROCESSES; i++) {
        if (waitpid(p[i], &status, 0) == ERROR_VALUE) {
            perror("Something went wrong while waiting for my child process child to terminate!\n");
            exit(EXIT_FAILURE);
        }

        /* Se o processo filho não terminar com sucesso */
        if (!WIFEXITED(status)) {
            perror("The child process did not finish successfully!\n");
            exit(EXIT_FAILURE);
        }
    }

    /* Liberta o semáforo responsável por imprimir uma mensagem de aviso sempre que um novo valor maior for encontrado no vetor final */
    sem_post_with_validation(sem[SEMAPHORE_INDEX_2]);
    /* Atualiza o valor da flag, visto que todos os filhos já terminaram de analisar o vetor inicial */
    shared_data->flag = 1;

    /* Imprime o vetor final */
    printf("Final vector: \n");
    for(i = 0; i < FINAL_VETOR_SIZE; i++) {
        printf("Position %d -> %d\n", i, shared_data->final_vector[i]);
    }

    /* Imprime o maior número encontrado no vetor final */
    printf("Greater value: %d\n", shared_data->max_number);

    /* Espera que o processo filho termine */
    if (wait(&status) == ERROR_VALUE) {
        perror("Something went wrong while waiting for my child process child to terminate!\n");
        exit(EXIT_FAILURE);
    }

    /* Se o processo filho não terminar com sucesso */
    if (!WIFEXITED(status)) {
        perror("The child process did not finish successfully!\n");
        exit(EXIT_FAILURE);
    }

    /* Se não der erro desconecta o pointer da memória partilhada */
    munmap_with_validations(shared_data, data_size);

    /* Se não der erro fecha a zona de memória partilhada */
    close_with_validations(fd);

	/* Se não der erro remove a zona de memória partilhada */
    shm_unlink_with_validations(SHARED_MEMORY_NAME);

    /* Se não der erro fecha e remove os semáforos */
    for (i = 0; i < NUMBER_OF_SEMAPHORES; i++) {
        semaphore_close_with_validation(sem[i]);
        semaphore_unlink_with_validation(semaphores_name[i]);
    }  

	return 0;
}