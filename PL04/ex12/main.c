#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>

#include "../libs/my_shared_memory_lib.h"
#include "../libs/my_semaphore_lib.h"

#define SHARED_MEMORY_NAME "/ex12_shared_memory"
#define EXCL_SEMAPHORE_NAME "/ex12_excl_semaphore"
#define ITEMS_SEMAPHORE_NAME "/ex12_items_semaphore"
#define SPACE_SEMAPHORE_NAME "/ex12_space_semaphore"

#define BUFFER_SIZE 10
#define NUM_VALUES_TO_HANDLE 30

#define ERROR_VALUE -1

typedef struct {
    int data[BUFFER_SIZE];          // Dados a guardar
    int write_index;                // Índice da posição para escrita atual
    int p1_finished_writing;        // Indica se o processo 1 terminou de escrever os valores
    int p2_finished_writing;        // Indica se o processo 2 terminou de escrever os valores
} buffer_data;

int main() {

    int fd;

    off_t data_size = sizeof(buffer_data); // Tamanho necessário para a memória partilhada

    buffer_data *shared_data;

    // Cria e abre a zona de memória partilhada
    fd = shm_open_with_validations(SHARED_MEMORY_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR|S_IWUSR);
    // Se não der erro no ftruncate define o tamanho da área da memória partilhada e inicializa-a a 0
    ftruncate_with_validations(fd, data_size);
    // Pointer para a memória partilhada
    shared_data = (buffer_data *) mmap_with_validations(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    shared_data->write_index = 0;
    shared_data->p1_finished_writing = 0;
    shared_data->p2_finished_writing = 0;

    // Abre o semáforo que irá controlar a exclusão mútua
    sem_t *excl_sem = semaphore_open_with_validation(EXCL_SEMAPHORE_NAME, O_CREAT | O_EXCL, 0644, 1);
    // Abre o semáforo que irá controlar o número de items que podem ser lidos do buffer
    sem_t *items_sem = semaphore_open_with_validation(ITEMS_SEMAPHORE_NAME, O_CREAT | O_EXCL, 0644, 0);
    // Abre o semáforo que irá controlar o espaço disponível no buffer
    sem_t *space_sem = semaphore_open_with_validation(SPACE_SEMAPHORE_NAME, O_CREAT | O_EXCL, 0644, BUFFER_SIZE);

    // Cria os dois processos filhos (produtores)
    pid_t p1 = 1, p2 = 1;
    p1 = fork();
    if (p1 != 0) {
        p2 = fork();

        if (p1 == ERROR_VALUE || p2 == ERROR_VALUE) {
            perror("An error occurred while trying to create child processes!\n");
            exit(EXIT_FAILURE);
        }
    }

    if (p1 == 0 || p2 == 0) { // Se for processo filho
        pid_t my_pid = getpid();
        int i = 0;

        while (i < NUM_VALUES_TO_HANDLE) {
            sem_wait_with_validation(space_sem); // Esperar passivamente que exista espaço livre para escrever no buffer
            sem_wait_with_validation(excl_sem); // Esperar passivamente para realizar a exclusão mútua e ganhar acesso exclusivo à zona crítica

            // Se o local a escrever já for maior do que o tamanho do buffer, significa que temos de voltar à posição inicial do buffer
            if (shared_data->write_index == BUFFER_SIZE)
                shared_data->write_index = 0;

            // Escreve o valor na posição write_index do buffer
            shared_data->data[shared_data->write_index] = i;
            printf("The process %d writed in the buffer the value %d!\n", my_pid, i);

            shared_data->write_index++; // Próxima posição do buffer onde será escrito o próximo valor

            sem_post_with_validation(excl_sem); // Libertar o acesso exclusivo à zona crítica
            sem_post_with_validation(items_sem); // Informar que existe mais 1 item novo a ser lido do buffer

            i++; // Próximo valor a ser escrito no buffer
            sleep(1); // Espera 1 segundo até tentar escrever novo valor
        }

        // Indicar que o processo p1 terminou de escrever todos os valores
        if (p1 == 0)
            shared_data->p1_finished_writing = 1;

        // Indicar que o processo p2 terminou de escrever todos os valores
        if (p2 == 0)
            shared_data->p2_finished_writing = 1;
        
    } else { // Se for o processo pai
        int space_sem_value = BUFFER_SIZE; // Espaço existente no buffer (valor do semáforo space)

        int read_index = 0;
        // Enquanto o processo p1 ou p2 não tiverem escrito todos os valores no buffer, ou o buffer ainda não estar completamente livre
        while ( (!shared_data->p1_finished_writing || !shared_data->p2_finished_writing) || space_sem_value < BUFFER_SIZE) {
            sem_wait_with_validation(items_sem); // Esperar passivamente para que exista um item a ser consumido
            sem_wait_with_validation(excl_sem); // Esperar passivamente para realizar a exclusão mútua e ganhar acesso exclusivo à zona crítica

            printf("Valor lido do buffer: %d\n", shared_data->data[read_index]);
            read_index++;
            // Se o índice a ler já for fora do limite do buffer, voltar ao início do buffer
            if (read_index == BUFFER_SIZE)
                read_index = 0;

            sem_post_with_validation(excl_sem); // Libertar o acesso exclusivo à zona crítica
            sem_post_with_validation(space_sem); // Informar que um valor foi consumido do buffer e que existe mais um espaço vazio

            sem_getvalue_with_validations(space_sem, &space_sem_value); // Guarda o valor do semáforo space_sem na variável space_sem_value

            sleep(2); // Espera 2 segundos até tentar ler novo valor
        }

        // Esperar que os processos filhos terminem
        int p1_status_exit, p2_status_exit;
        if (waitpid(p1, &p1_status_exit, 0) == ERROR_VALUE || waitpid(p2, &p2_status_exit, 0) == ERROR_VALUE) {
            perror("An error occurred while waiting for child processes to terminate!\n");
            exit(EXIT_FAILURE);
        }

        // Se os processos filhos não terminaram com sucesso
        if (!WIFEXITED(p1_status_exit) || !WIFEXITED(p2_status_exit)) {
            perror("The child process did not finish successfully!\n");
            exit(EXIT_FAILURE);
        }
        
        // Remover a zona de memória partilhada e os semáforos criados da pasta /dev/shm
        shm_unlink_with_validations(SHARED_MEMORY_NAME);
        semaphore_unlink_with_validation(EXCL_SEMAPHORE_NAME);
        semaphore_unlink_with_validation(ITEMS_SEMAPHORE_NAME);
        semaphore_unlink_with_validation(SPACE_SEMAPHORE_NAME);

    }

    // Fechar a zona de memória partilhada
    munmap_with_validations(shared_data, data_size);
    close_with_validations(fd);

    // Fechar os semáforos abertos
    semaphore_close_with_validation(excl_sem);
    semaphore_close_with_validation(items_sem);
    semaphore_close_with_validation(space_sem);

    return 0;

}
