#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>

#include "../../libs/my_shared_memory_lib.h"
#include "../../libs/my_semaphore_lib.h"

#define SHARED_MEMORY_NAME "/ex02_shared_memory"
#define SEMAPHORE_NAME "/ex02_semaphore"
#define STRING_LENGTH 80
#define NUMBER_OF_STRINGS 50
#define MAX_WAITING_SECONDS 12
#define REMOVE_LAST_WRITTEN_STRING_PROBABILITY 30

int main() {

    char string_to_write[STRING_LENGTH];
    sprintf(string_to_write, "I'm the Father - with PID %d", getpid()); // String a colocar na memória partilhada

    int fd;

    off_t data_size = sizeof(char) * STRING_LENGTH * NUMBER_OF_STRINGS; // Tamanho necessário para a memória partilhada

    char *shared_data;

    // Cria e abre a zona de memória partilhada
    fd = shm_open_with_validations(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, S_IRUSR|S_IWUSR);
    // Se não der erro no ftruncate define o tamanho da área da memória partilhada e inicializa-a a 0
    ftruncate_with_validations(fd, data_size);
    // Pointer para a memória partilhada
    shared_data = (char *) mmap_with_validations(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    char *temp_shared_data;
    temp_shared_data = shared_data;

    // Abre o semáforo que irá controlar a exclusão mútua
    sem_t *sem = semaphore_open_with_validation(SEMAPHORE_NAME, O_CREAT, 0644, 1);

    srand(time(NULL));
    
    struct timespec timeout_waiting;

    int pos = 0;
    while (pos < NUMBER_OF_STRINGS) {

        // Calcula o intervalo relativo como a hora atual mais o número máximo de segundos.
        if (clock_gettime(CLOCK_REALTIME, &timeout_waiting) == -1) {
            perror("Something went wrong while doing the clock_gettime!\n");
            exit(EXIT_FAILURE);
        }
        timeout_waiting.tv_sec += MAX_WAITING_SECONDS;
        int timeout_result = sem_timedwait(sem, &timeout_waiting); // Ganhar acesso à zona crítica
        
        if (timeout_result == -1) {
            if (errno == ETIMEDOUT) {
                printf("Warning: The limit access time to the shared memory expired! (%d seconds)\n", MAX_WAITING_SECONDS);
            } else {
                perror("Something went wrong while doing the sem_timedwait()!\n");
                exit(EXIT_FAILURE);
            }
        } else {
            while (pos < NUMBER_OF_STRINGS && *temp_shared_data != '\0') {
                temp_shared_data += STRING_LENGTH; // Ir para o local do início da próxima string
                pos++;
            }
            
            // Para prevenir que ao chegar ao final quando não existe mais strings vazias, ele não escreva em zonas de memória indesejadas
            if (pos < NUMBER_OF_STRINGS) {
                sprintf(temp_shared_data, string_to_write); // Escrever na zona de memória a string desejada
                int random_number = rand() % 100 + 1; // Valor aleatório entre 1 e 100
                // Probabilidade de 30% de remover a última linha escrita na zona crítica
                if (random_number < REMOVE_LAST_WRITTEN_STRING_PROBABILITY) {
                    *temp_shared_data = '\0'; // Indica que a String neste local está teoricamente apagada
                    printf("%s\n", string_to_write);
                }
            }

            sleep((rand() % 5) + 1); // Dormir durante 1 a 5 segundos (tempo aleatório)
            sem_post_with_validation(sem); // Libertar a zona crítica
            sleep((rand() % 5) + 1);
        }

    }

    munmap_with_validations(shared_data, data_size);
    close_with_validations(fd);
    //shm_unlink_with_validations(SHARED_MEMORY_NAME);
    semaphore_close_with_validation(sem);
    //semaphore_unlink_with_validation(SEMAPHORE_NAME);

}
