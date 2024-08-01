#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <sys/wait.h>

#include "../../libs/my_shared_memory_lib.h"
#include "../../libs/my_semaphore_lib.h"
#include "bar.h"

#define BAR_SHARED_MEMORY_NAME "/ex15_bar_shared_memory"
#define SEMAPHORE_SPACE_IN_QUEUE "/ex15_semaphore_space_in_queue"
#define SEMAPHORE_EXCL "/ex15_semaphore_excl"
#define SEMAPHORE_CLIENTS_IN_QUEUE "/ex15_semaphore_clients_in_queue"

#define SEMAPHORE_SPACE_IN_BAR "/ex15_semaphore_space_in_bar"

#define ERROR_VALUE -1
#define MAX_TIME_TO_WAIT_FOR_A_CLIENT 20
#define NUM_CLIENTS 50
#define BAR_SPACE 10

int main() {
    // Criação da memória partilhada para controlar o bar
    int fd_bar;
    off_t bar_data_size = sizeof(bar); // Tamanho necessário para a memória partilhada
    bar *bar_shared_data;
    // Cria e abre a zona de memória partilhada
    fd_bar = shm_open_with_validations(BAR_SHARED_MEMORY_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    // Define o tamanho da área da memória partilhada e inicializa-a a 0
    ftruncate_with_validations(fd_bar, bar_data_size);
    // Pointer para a memória partilhada
    bar_shared_data = (bar *) mmap_with_validations(NULL, bar_data_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd_bar, 0);

    bar_shared_data->next_queue_client_index = 0;
    bar_shared_data->next_client_to_enter_in_bar = 0;

    // Cria o semáforo que conhece o espaço total atual da fila de espera
    sem_t *sem_space_in_queue = semaphore_open_with_validation(SEMAPHORE_SPACE_IN_QUEUE, O_CREAT, 0644, NUM_MAX_CLIENTS_IN_QUEUE);
    // Cria o semáforo que irá controlar a exclusão mútua
    sem_t *sem_excl = semaphore_open_with_validation(SEMAPHORE_EXCL, O_CREAT, 0644, 1);
    // Cria o semáforo que sabe quantos clientes estão à espera de entrar no bar na fila de espera
    sem_t *sem_clients_in_queue = semaphore_open_with_validation(SEMAPHORE_CLIENTS_IN_QUEUE, O_CREAT, 0644, 0);
    // Cria o semáforo que controla a lotação dentro do bar
    sem_t *sem_space_in_bar = semaphore_open_with_validation(SEMAPHORE_SPACE_IN_BAR, O_CREAT, 0644, BAR_SPACE);

    // Cria os semáforos na memória partilhada, que serão usados para indicar que um cliente numa dada posição já pode entrar
    for (int i = 0; i < NUM_MAX_CLIENTS_IN_QUEUE; i++) {
        char sem_name[33];
        sprintf(sem_name, "/ex15_sempahore_client_%d", i);
        bar_shared_data->clients_queue[i] = semaphore_open_with_validation(sem_name, O_CREAT, 0644, 0);
    }

    // Criar os processos filho (clientes)
    pid_t p;
    for (int i = 0; i < NUM_CLIENTS; i++) {
        p = fork();
        /* Caso aconteça uma falha na criação do processo filho */
        if (p == ERROR_VALUE) {
            perror("Fork failed!\n");
            exit(EXIT_FAILURE);
        }

        if (p == 0) // Se for processo filho
            break;
    }

    // Se for o processo pai
    if (p > 0) {
        struct timespec timeout_waiting;
        
        while (1) {
            // Calcula o intervalo relativo com a hora atual mais o número máximo de segundos.
            if (clock_gettime(CLOCK_REALTIME, &timeout_waiting) == -1) {
                perror("Something went wrong while doing the clock_gettime!\n");
                exit(EXIT_FAILURE);
            }

            timeout_waiting.tv_sec += MAX_TIME_TO_WAIT_FOR_A_CLIENT;

            // Espera no máximo MAX_TIME_TO_WAIT_FOR_A_CLIENT para decrementar o semáforo. (espera que clientes entrem na fila de espera)
            int timeout_result = sem_timedwait(sem_clients_in_queue, &timeout_waiting);
            
            if (timeout_result == -1) {
                // Se o tempo esgotar
                if (errno == ETIMEDOUT) {
                    printf("Vou fechar o bar, por hoje já não devem aparecer mais clientes, pois já não chegam clientes novos á %d segundos! Não vou ficar aberto para as moscas! :D\n", MAX_TIME_TO_WAIT_FOR_A_CLIENT);
                    break;
                } else { // Se der erro na função
                    perror("Something went wrong while doing the sem_timedwait()!\n");
                    exit(EXIT_FAILURE);
                }
            } else {
                int space_in_bar;
                sem_getvalue_with_validations(sem_space_in_bar, &space_in_bar);
                int clients_inside_bar = BAR_SPACE - space_in_bar;
                printf("\n**Bar info:** Clientes dentro do bar atualmente: %d\n\n", clients_inside_bar);

                // Espera que haja espaço dentro do bar
                sem_wait_with_validation(sem_space_in_bar);
                //sem_wait_with_validation(sem_excl);

                printf("\n**Bar info:** Vai entrar agora um cliente dentro do bar!\n\n");
                
                // Indica o próximo cliente que já pode entrar
                sem_post_with_validation(bar_shared_data->clients_queue[bar_shared_data->next_client_to_enter_in_bar]);

                // Calcula a posição do buffer circular onde se encontra o próximo cliente
                bar_shared_data->next_client_to_enter_in_bar++;
                if (bar_shared_data->next_client_to_enter_in_bar == NUM_MAX_CLIENTS_IN_QUEUE)
                    bar_shared_data->next_client_to_enter_in_bar = 0;

                //sem_post_with_validation(sem_excl);

                sleep(1); // Para garantir que os processos sinalizados entram por essa ordem e nenhum é atropelado
            }

        }

        // Espera que todos os filhos saiam do bar
        for (int i = 0; i < NUM_CLIENTS; i++) {
            if (wait(NULL) == ERROR_VALUE) {
                perror("Error while trying to wait for child process!\n");
                exit(EXIT_FAILURE);
            }
        }
        
        // Sinaliza a remoção da zona crítica
        shm_unlink_with_validations(BAR_SHARED_MEMORY_NAME);

        // Sinaliza a remoção dos semáforos
        semaphore_unlink_with_validation(SEMAPHORE_SPACE_IN_QUEUE);
        semaphore_unlink_with_validation(SEMAPHORE_EXCL);
        semaphore_unlink_with_validation(SEMAPHORE_CLIENTS_IN_QUEUE);
        semaphore_unlink_with_validation(SEMAPHORE_SPACE_IN_BAR);

        for (int i = 0; i < NUM_MAX_CLIENTS_IN_QUEUE; i++) {
            char sem_name[33];
            sprintf(sem_name, "/ex15_sempahore_client_%d", i);
            semaphore_unlink_with_validation(sem_name);
        }

    } else { // Se for o processo filho
        pid_t pid = getpid();
        srand(pid);
        
        // Espera que exista espaço na fila de espera
        sem_wait_with_validation(sem_space_in_queue);
        // Ganha acesso exclusívo à zona crítica
        sem_wait_with_validation(sem_excl);

        // Guarda a sua posição na fila de espera
        int pos = bar_shared_data->next_queue_client_index;

        // Trata a posição da fila de espera para o próximo cliente
        bar_shared_data->next_queue_client_index++;
        if (bar_shared_data->next_queue_client_index == NUM_MAX_CLIENTS_IN_QUEUE)
            bar_shared_data->next_queue_client_index = 0;

        // Liberta a zona crítica
        sem_post_with_validation(sem_excl);
        // Indica que existe mais um cliente à espera na fila de espera
        sem_post_with_validation(sem_clients_in_queue);

        printf("**Cliente com o pid %d:** Olá! Entrei na posição %d da fila de espera!\n", pid, pos);

        // Espera pela sua vez de entrar no bar
        sem_wait_with_validation(bar_shared_data->clients_queue[pos]);

        printf("**Cliente com o pid %d:** Entrei agora no bar! Estava na posição %d da fila de espera!\n", pid, pos);

        // Após entrar no bar, indica que existe mais um lugar vazio na fila de espera
        sem_post_with_validation(sem_space_in_queue);

        // Simula o tempo dentro do bar
        int time_in_bar = rand() % 20 + 1;
        sleep(time_in_bar);

        printf("**Cliente com o pid %d:** Vou embora, estive %d segundos no bar!\n", pid, time_in_bar);

        // Informa que existe mais um espaço dentro do bar, ou seja, que saiu um cliente
        sem_post_with_validation(sem_space_in_bar);

    }

    // Fechar os semáforos
    semaphore_close_with_validation(sem_space_in_bar);
    semaphore_close_with_validation(sem_excl);
    semaphore_close_with_validation(sem_clients_in_queue);
    semaphore_close_with_validation(sem_space_in_queue);

    for (int i = 0; i < NUM_MAX_CLIENTS_IN_QUEUE; i++) {
        semaphore_close_with_validation(bar_shared_data->clients_queue[i]);
    }

    // Fechar a zona crítica
    munmap_with_validations(bar_shared_data, bar_data_size);
    close_with_validations(fd_bar);
}
