#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

#include "../../libs/my_shared_memory_lib.h"
#include "../../libs/my_semaphore_lib.h"
#include "../client_struct.h"
#include "../functional_data_struct.h"

#define CLIENTS_SHARED_MEMORY_NAME "/ex08_clients_shared_memory"
#define READERS_PROC_SHARED_MEMORY_NAME "/ex08_readers_proc_shared_memory"
#define SEMAPHORE_TORNIQUETE "/ex08_semaphore_torniquete"
#define SEMAPHORE_EXCL "/ex08_semaphore_excl"
#define SEMAPHORE_NO_USE "/ex08_semaphore_no_use"
#define NUM_MAX_CLIENTS 100

int main() {

    int fd_readers_proc;
    off_t readers_proc_data_size = sizeof(functional_data); // Tamanho necessário para a memória partilhada
    functional_data *functional_shared_data;
    // Cria e abre a zona de memória partilhada
    fd_readers_proc = shm_open_with_validations(READERS_PROC_SHARED_MEMORY_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    // Se não der erro no ftruncate define o tamanho da área da memória partilhada e inicializa-a a 0
    ftruncate_with_validations(fd_readers_proc, readers_proc_data_size);
    // Pointer para a memória partilhada
    functional_shared_data = (functional_data *) mmap_with_validations(NULL, readers_proc_data_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd_readers_proc, 0);

    // Se não existir clientes registados no sistema não vale a pena avançar
    if (functional_shared_data->num_clients == 0) {
        printf("There is no clients registered in the system!\n");
        return 0;
    }

    int fd_clients;
    off_t clients_data_size = sizeof(client_data) * NUM_MAX_CLIENTS; // Tamanho necessário para a memória partilhada
    client_data *clients_shared_data;
    // Cria e abre a zona de memória partilhada
    fd_clients = shm_open_with_validations(CLIENTS_SHARED_MEMORY_NAME, O_RDONLY, S_IRUSR);
    // Pointer para a memória partilhada
    clients_shared_data = (client_data *) mmap_with_validations(NULL, clients_data_size, PROT_READ, MAP_SHARED, fd_clients, 0);

    // Abre o semáforo que irá controlar a prioridade da escrita sobre a leitura
    sem_t *sem_torniquete = semaphore_open_with_validation(SEMAPHORE_TORNIQUETE, O_CREAT, 0644, 1);
    // Abre o semáforo que irá controlar a exclusão mútua
    sem_t *sem_excl = semaphore_open_with_validation(SEMAPHORE_EXCL, O_CREAT, 0644, 1);
    // Abre o semáforo que controla quando é possível escrever na zona crítica
    sem_t *sem_no_use = semaphore_open_with_validation(SEMAPHORE_NO_USE, O_CREAT, 0644, 1);

    // Verifica se não existe nenhum escritor a querer escrever
    sem_wait_with_validation(sem_torniquete);
    sem_post_with_validation(sem_torniquete);

    sem_wait_with_validation(sem_excl); // Exclusão mútua
    functional_shared_data->num_readers++; // Informa que existe mais um leitor a querer ler da zona crítica
    // Se for o primeiro leitor, informa que se algum processo escritor tentar escrever vai ter de aguardar até que as leituras terminem
    if (functional_shared_data->num_readers == 1)
        sem_wait_with_validation(sem_no_use);
    sem_post_with_validation(sem_excl); // Fim da exclusão mútua

    client_data *temp_clients_shared_data;
    temp_clients_shared_data = clients_shared_data;
    
    // Realiza a leitura da zona crítica e imprime no ecrã os dados dos clientes
    int count = 0;
    while (count < functional_shared_data->num_clients) {
        sleep(1); // Simular o tempo que demora a imprimir os dados de um cliente (1 segundo)
        printf("Id number: %d\n", temp_clients_shared_data->id_number);
        printf("Name: %s\n", temp_clients_shared_data->name);
        printf("Address: %s\n\n", temp_clients_shared_data->address);
        temp_clients_shared_data++;
        count++;
    }

    sem_wait_with_validation(sem_excl); // Exclusão mútua
    functional_shared_data->num_readers--; // Informa que existe menos um leitor a ler da zona crítica
    // Se for o último leitor, informa que se algum processo escritor tentar escrever já o poderá fazer
    if (functional_shared_data->num_readers == 0)
        sem_post_with_validation(sem_no_use);
    sem_post_with_validation(sem_excl); // Fim da exclusão mútua

    // Fechar a zona crítica com os dados dos clientes
    munmap_with_validations(clients_shared_data, clients_data_size);
    close_with_validations(fd_clients);

    // Fechar a zona crítica com os dados funcionais
    munmap_with_validations(functional_shared_data, readers_proc_data_size);
    close_with_validations(fd_readers_proc);

    // Fechar os semáforos
    semaphore_close_with_validation(sem_torniquete);
    semaphore_close_with_validation(sem_excl);
    semaphore_close_with_validation(sem_no_use);


}
