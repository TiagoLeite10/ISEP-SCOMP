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

#include "../../libs/my_shared_memory_lib.h"
#include "../../libs/my_semaphore_lib.h"
#include "bar.h"

#define BAR_SHARED_MEMORY_NAME "/ex15_bar_shared_memory"
#define SEMAPHORE_SPACE_IN_NORMAL_QUEUE "/ex15_semaphore_space_in_normal_queue"
#define SEMAPHORE_SPACE_IN_SPECIAL_QUEUE "/ex15_semaphore_space_in_special_queue"
#define SEMAPHORE_SPACE_IN_VIP_QUEUE "/ex15_semaphore_space_in_vip_queue"
#define SEMAPHORE_EXCL "/ex15_semaphore_excl"
#define SEMAPHORE_CLIENTS_IN_QUEUE "/ex15_semaphore_clients_in_queue"
#define SEMAPHORE_SPACE_IN_BAR "/ex15_semaphore_space_in_bar"
#define SEMAPHORE_NORMAL_CLIENTS "/ex15_semaphore_normal_clients"
#define SEMAPHORE_SPECIAL_CLIENTS "/ex15_semaphore_special_clients"
#define SEMAPHORE_VIP_CLIENTS "/ex15_semaphore_vip_clients"

#define NUM_CLIENTS_TO_ENTER_IN_BAR 50
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

    // Cria o semáforo que conhece o espaço total atual da fila de espera
    sem_t *sem_space_in_normal_queue = semaphore_open_with_validation(SEMAPHORE_SPACE_IN_NORMAL_QUEUE, O_CREAT, 0644, NUM_MAX_CLIENTS_IN_QUEUE);
    // Cria o semáforo que conhece o espaço total atual da fila de espera
    sem_t *sem_space_in_special_queue = semaphore_open_with_validation(SEMAPHORE_SPACE_IN_SPECIAL_QUEUE, O_CREAT, 0644, NUM_MAX_CLIENTS_IN_QUEUE);
    // Cria o semáforo que conhece o espaço total atual da fila de espera
    sem_t *sem_space_in_vip_queue = semaphore_open_with_validation(SEMAPHORE_SPACE_IN_VIP_QUEUE, O_CREAT, 0644, NUM_MAX_CLIENTS_IN_QUEUE);
    // Cria o semáforo que irá controlar a exclusão mútua
    sem_t *sem_excl = semaphore_open_with_validation(SEMAPHORE_EXCL, O_CREAT, 0644, 1);
    // Cria o semáforo que sabe quantos clientes estão à espera de entrar no bar na fila de espera
    sem_t *sem_clients_in_queue = semaphore_open_with_validation(SEMAPHORE_CLIENTS_IN_QUEUE, O_CREAT, 0644, 0);
    // Cria o semáforo que controla a lotação dentro do bar
    sem_t *sem_space_in_bar = semaphore_open_with_validation(SEMAPHORE_SPACE_IN_BAR, O_CREAT, 0644, BAR_SPACE);

    // Semáforos que indicam se um existe um cliente de um dado tipo à espera
    sem_t *sem_clients[NUM_OF_CLIENTS_TYPE];
    // Semáforo que sabe quantos clientes normais estão à espera de entrar
    sem_clients[NORMAL_CLIENT_INDEX] = semaphore_open_with_validation(SEMAPHORE_NORMAL_CLIENTS, O_CREAT, 0644, 0);
    // Semáforo que sabe quantos clientes especiais estão à espera de entrar
    sem_clients[SPECIAL_CLIENT_INDEX] = semaphore_open_with_validation(SEMAPHORE_SPECIAL_CLIENTS, O_CREAT, 0644, 0);
    // Semáforo que sabe quantos clientes VIPs estão à espera de entrar
    sem_clients[VIP_CLIENT_INDEX] = semaphore_open_with_validation(SEMAPHORE_VIP_CLIENTS, O_CREAT, 0644, 0);

    // Cria os semáforos na memória partilhada, que serão usados para indicar que um dado cliente de um dado tipo já pode entrar
    // Cada tipo de cliente tem a sua fila
    for (int i = 0; i < NUM_MAX_CLIENTS_IN_QUEUE; i++) {
        char sem_name[33];
        sprintf(sem_name, "/ex15_sempahore_normal_client_%d", i);
        bar_shared_data->clients_queue[NORMAL_CLIENT_INDEX][i] = semaphore_open_with_validation(sem_name, O_CREAT, 0644, 0);

        sprintf(sem_name, "/ex15_sempahore_special_client_%d", i);
        bar_shared_data->clients_queue[SPECIAL_CLIENT_INDEX][i] = semaphore_open_with_validation(sem_name, O_CREAT, 0644, 0);

        sprintf(sem_name, "/ex15_sempahore_vip_client_%d", i);
        bar_shared_data->clients_queue[VIP_CLIENT_INDEX][i] = semaphore_open_with_validation(sem_name, O_CREAT, 0644, 0);
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

        if (p == 0) // Se for o processo filho
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
            int timeout_result = sem_timedwait(sem_clients_in_queue, &timeout_waiting); // Ganhar acesso à zona crítica
            
            if (timeout_result == -1) {
                // Se o tempo esgotar
                if (errno == ETIMEDOUT) {
                    printf("\nVou fechar o bar, por hoje já não devem aparecer mais clientes, pois já não chegam clientes novos á %d segundos! Não vou ficar aberto para as moscas! :D\n", MAX_TIME_TO_WAIT_FOR_A_CLIENT);
                    break;
                } else { // Se der erro na função
                    perror("Something went wrong while doing the sem_timedwait()!\n");
                    exit(EXIT_FAILURE);
                }
            } else {

                int space_in_bar;
                sem_getvalue_with_validations(sem_space_in_bar, &space_in_bar);
                int clients_inside_bar = BAR_SPACE - space_in_bar;
                printf("\n**Bar info:** Número de clientes dentro do bar: %d! Estou à espera que exista espaço para atender mais clientes caso estejam na fila de espera! :)\n\n", clients_inside_bar);

                sem_wait_with_validation(sem_space_in_bar);
                printf("\n**Bar info:** Pode entrar mais um cliente no bar!\n\n");
                //sem_wait_with_validation(sem_excl);

                // Indica se já foi encontrado o tipo de cliente do próximo cliente
                int type_client_found = 0;
                // Posição do próximo cliente
                int next_client;
                // Tipo de cliente que vai entrar
                int type_client = ERROR_VALUE;

                // Verifica se o cliente é do tipo vip
                int try_wait_for_vip_client = sem_trywait(sem_clients[VIP_CLIENT_INDEX]);
                if (try_wait_for_vip_client == 0) {
                    next_client = bar_shared_data->next_client_to_enter_in_bar[VIP_CLIENT_INDEX];
                    type_client = VIP_CLIENT_INDEX;
                    sem_post_with_validation(bar_shared_data->clients_queue[type_client][next_client]); // Avisar o cliente VIP para entrar
                    type_client_found = 1;
                } else if (try_wait_for_vip_client == ERROR_VALUE) {
                    if (errno == EINVAL) {
                        perror("Something went wrong with VIP client semaphore!\n");
                        exit(EXIT_FAILURE);
                    }
                }

                // Verifica se o cliente é do tipo especial
                if (type_client_found == 0) {
                    int try_wait_for_special_client = sem_trywait(sem_clients[SPECIAL_CLIENT_INDEX]);
                    if (try_wait_for_special_client == 0) {
                        next_client = bar_shared_data->next_client_to_enter_in_bar[SPECIAL_CLIENT_INDEX];
                        type_client = SPECIAL_CLIENT_INDEX;
                        sem_post_with_validation(bar_shared_data->clients_queue[type_client][next_client]); // Avisar o cliente especial para entrar
                        type_client_found = 1;
                    } else if (try_wait_for_special_client == ERROR_VALUE) {
                        if (errno == EINVAL) {
                            perror("Something went wrong with SPECIAL client semaphore!\n");
                            exit(EXIT_FAILURE);
                        }
                    }    
                }

                // Verifica se o cliente é do tipo normal
                if (type_client_found == 0) {
                    int try_wait_for_normal_client = sem_trywait(sem_clients[NORMAL_CLIENT_INDEX]);
                    if (try_wait_for_normal_client == 0) {
                        next_client = bar_shared_data->next_client_to_enter_in_bar[NORMAL_CLIENT_INDEX];
                        type_client = NORMAL_CLIENT_INDEX;
                        sem_post_with_validation(bar_shared_data->clients_queue[type_client][next_client]); // Avisar o cliente normal para entrar
                        type_client_found = 1;

                    } else if (try_wait_for_normal_client == ERROR_VALUE) {
                        if (errno == EINVAL) {
                            perror("Something went wrong with NORMAL client semaphore!\n");
                            exit(EXIT_FAILURE);
                        }
                    }
                }

                // Para ter a certeza que tudo funciona
                if (type_client == ERROR_VALUE || type_client_found == 0) {
                    perror("Error in the semaphore client types logic!\n");
                    exit(EXIT_FAILURE);
                }

                // Tratar da posição em que se encontra o próximo cliente da fila do tipo de cliente que entrou
                bar_shared_data->next_client_to_enter_in_bar[type_client]++;
                if (bar_shared_data->next_client_to_enter_in_bar[type_client] == NUM_MAX_CLIENTS_IN_QUEUE)
                    bar_shared_data->next_client_to_enter_in_bar[type_client] = 0;

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

        // Sinaliza a remoção dos semáforos
        semaphore_unlink_with_validation(SEMAPHORE_NORMAL_CLIENTS);
        semaphore_unlink_with_validation(SEMAPHORE_SPECIAL_CLIENTS);
        semaphore_unlink_with_validation(SEMAPHORE_VIP_CLIENTS);
        semaphore_unlink_with_validation(SEMAPHORE_SPACE_IN_NORMAL_QUEUE);
        semaphore_unlink_with_validation(SEMAPHORE_SPACE_IN_SPECIAL_QUEUE);
        semaphore_unlink_with_validation(SEMAPHORE_SPACE_IN_VIP_QUEUE);
        semaphore_unlink_with_validation(SEMAPHORE_EXCL);
        semaphore_unlink_with_validation(SEMAPHORE_CLIENTS_IN_QUEUE);
        semaphore_unlink_with_validation(SEMAPHORE_SPACE_IN_BAR);

        for (int i = 0; i < NUM_MAX_CLIENTS_IN_QUEUE; i++) {
            char sem_name[33];
            sprintf(sem_name, "/ex15_sempahore_normal_client_%d", i);
            semaphore_unlink_with_validation(sem_name);

            sprintf(sem_name, "/ex15_sempahore_special_client_%d", i);
            semaphore_unlink_with_validation(sem_name);

            sprintf(sem_name, "/ex15_sempahore_vip_client_%d", i);
            semaphore_unlink_with_validation(sem_name);
        }

        // Sinaliza a remoção da zona crítica
        shm_unlink_with_validations(BAR_SHARED_MEMORY_NAME);

    } else { // Se for o processo filho
        pid_t pid = getpid();
        srand(pid);

        char client_type_description[9];
        sem_t *sem_temp_queue; // Vai ficar com o semáforo da fila do tipo de cliente que este vai ser

        // Decidir o tipo de cliente de forma aleatória
        int client_type;
        switch (rand() % 3) {
            case 0:
                client_type = NORMAL_CLIENT_INDEX;
                strcpy(client_type_description, "NORMAL");
                sem_temp_queue = sem_space_in_normal_queue;
                break;
            case 1:
                client_type = SPECIAL_CLIENT_INDEX;
                strcpy(client_type_description, "SPECIAL");
                sem_temp_queue = sem_space_in_special_queue;
                break;
            case 2: 
                client_type = VIP_CLIENT_INDEX;
                strcpy(client_type_description, "VIP");
                sem_temp_queue = sem_space_in_vip_queue;
                break;
        }

        // Espera que exista espaço na fila de espera
        sem_wait_with_validation(sem_temp_queue);
        // Ganha acesso exclusívo à zona crítica
        sem_wait_with_validation(sem_excl);

        // Guarda a sua posição na fila de espera
        int pos = bar_shared_data->next_queue_client_index[client_type];

        // Trata a posição da fila de espera para o próximo cliente
        bar_shared_data->next_queue_client_index[client_type]++;
        if (bar_shared_data->next_queue_client_index[client_type] == NUM_MAX_CLIENTS_IN_QUEUE)
            bar_shared_data->next_queue_client_index[client_type] = 0;

        // Liberta a zona crítica
        sem_post_with_validation(sem_excl);
        
        // Indica que existe mais um cliente à espera na fila de espera (fila de espera do respetivo tipo de cliente)
        sem_post_with_validation(sem_clients[client_type]);

        // Informa que existe mais um cliente que quer entrar no bar
        sem_post_with_validation(sem_clients_in_queue);

        printf("**Cliente com o pid %d e do tipo %s:** Olá! Entrei na posição %d da fila de espera!\n", pid, client_type_description, pos);

        // Espera pela sua vez de entrar
        sem_wait_with_validation(bar_shared_data->clients_queue[client_type][pos]);

        printf("**Cliente com o pid %d e do tipo %s:** Entrei agora no bar! Estava na posição %d da fila de espera!\n", pid, client_type_description, pos);
        
        // Após entrar no bar, indica que existe mais um lugar vazio na fila de espera
        sem_post_with_validation(sem_temp_queue);

        // Simula o tempo dentro do bar
        int time_in_bar = rand() % 20 + 1;
        sleep(time_in_bar);
        printf("**Cliente com o pid %d e do tipo %s:** Vou embora, estive %d segundos no bar!\n", pid, client_type_description, time_in_bar);

        // Informa que existe mais um espaço dentro do bar, ou seja, que saiu um cliente
        sem_post_with_validation(sem_space_in_bar);

        int space_in_bar;
        sem_getvalue_with_validations(sem_space_in_bar, &space_in_bar);
        int clients_inside_bar = BAR_SPACE - space_in_bar;
        printf("**Cliente com o pid %d e do tipo %s:** Sai agora do bar e ficaram lá %d clientes!\n", pid, client_type_description, clients_inside_bar);

    }

    // Fecha todos os semáforos utilizados
    for (int i = 0; i < NUM_MAX_CLIENTS_IN_QUEUE; i++) {
        semaphore_close_with_validation(bar_shared_data->clients_queue[NORMAL_CLIENT_INDEX][i]);
        semaphore_close_with_validation(bar_shared_data->clients_queue[SPECIAL_CLIENT_INDEX][i]);
        semaphore_close_with_validation(bar_shared_data->clients_queue[VIP_CLIENT_INDEX][i]);
    }
    
    for (int i = 0; i < NUM_OF_CLIENTS_TYPE; i++) {
        semaphore_close_with_validation(sem_clients[i]);
    }

    semaphore_close_with_validation(sem_space_in_normal_queue);
    semaphore_close_with_validation(sem_space_in_special_queue);
    semaphore_close_with_validation(sem_space_in_vip_queue);
    semaphore_close_with_validation(sem_excl);
    semaphore_close_with_validation(sem_clients_in_queue);
    semaphore_close_with_validation(sem_space_in_bar);
    
    // Fechar a zona de memória partilhada utilizada
    munmap_with_validations(bar_shared_data, bar_data_size);
    close_with_validations(fd_bar);

}
