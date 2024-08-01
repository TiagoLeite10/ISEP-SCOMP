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

#include "../libs/my_shared_memory_lib.h"
#include "../libs/my_semaphore_lib.h"
#include "sensor_data_to_controller.h"
#include "sensor.h"

#define SENSOR_DATA_TO_CONTROLLER_SHARED_MEMORY_NAME "/ex17_sensor_data_to_controller_shared_memory"
#define WRITE_INDEX_SHARED_MEMORY_NAME "/ex17_write_index_shared_memory"

#define SEMAPHORE_SPACE "/ex17_semaphore_sem_space"
#define SEMAPHORE_EXCL "/ex17_semaphore_sem_excl"
#define SEMAPHORE_DATA_TO_READ "/ex17_semaphore_sem_data_to_read"

#define NUM_SENSORS 5
#define NUM_READS_BY_EACH_SENSOR 6
#define ERROR_VALUE -1

int main() {

    // Criação da zona de memória partilhada para guardar os dados lidos pelos sensores
    int fd_sensor_data_to_controller;
    off_t sensor_data_to_controller_data_size = sizeof(sensor_data_to_controller) * NUM_SENSORS; // Tamanho necessário para a memória partilhada
    sensor_data_to_controller *sensor_data_to_controller_shared_data;
    // Cria e abre a zona de memória partilhada
    fd_sensor_data_to_controller = shm_open_with_validations(SENSOR_DATA_TO_CONTROLLER_SHARED_MEMORY_NAME, O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    // Define o tamanho da área da memória partilhada e inicializa-a a 0
    ftruncate_with_validations(fd_sensor_data_to_controller, sensor_data_to_controller_data_size);
    // Pointer para a memória partilhada
    sensor_data_to_controller_shared_data = (sensor_data_to_controller *) mmap_with_validations(NULL, sensor_data_to_controller_data_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd_sensor_data_to_controller, 0);
    
    // Criação da zona de memória partilhada para os processos filhos (sensores) saberem entre sí onde devem escrever os dados lidos
    int fd_write_index;
    off_t write_index_data_size = sizeof(int);
    fd_write_index = shm_open_with_validations(WRITE_INDEX_SHARED_MEMORY_NAME, O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate_with_validations(fd_write_index, write_index_data_size);
    int *write_index = (int *) mmap_with_validations(NULL, write_index_data_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd_write_index, 0);

    // Semáforo que sabe o espaço existente no buffer circular
    sem_t *sem_space = semaphore_open_with_validation(SEMAPHORE_SPACE, O_CREAT, 0644, NUM_SENSORS);
    // Semáforo para controlar a exclusão mútua
    sem_t *sem_excl = semaphore_open_with_validation(SEMAPHORE_EXCL, O_CREAT, 0644, 1);
    // Semáforo que indica a quantidade de dados que podem ser lidos do buffer circular no momento
    sem_t *sem_data_to_read = semaphore_open_with_validation(SEMAPHORE_DATA_TO_READ, O_CREAT, 0644, 0);

    int i;
    pid_t p;
    for (i = 0; i < NUM_SENSORS; i++) { // Criar os processos filho
        p = fork();
        /* Caso aconteça uma falha na criação do processo filho */
        if (p == ERROR_VALUE) {
            perror("Fork failed!\n");
            exit(EXIT_FAILURE);
        }

        if (p == 0) // Se for processo filho
            break;
    }

    // Se for o processo pai (controller)
    if (p > 0) {
        int num_sensors_in_alarm = 0; // Número de sensores em modo alarme
        int total_reads = NUM_SENSORS * NUM_READS_BY_EACH_SENSOR; // Total de leituras a fazer pelos sensores
        int read_index = 0; // Índice de leitura na zona crítica

        while (total_reads) {
            sem_wait_with_validation(sem_data_to_read); // Espera que haja dados para ler da zona crítica
            //sem_wait_with_validation(sem_excl);

            // Apontador para a zona da memória partilhada a ser lida
            sensor_data_to_controller *tem_ptr = sensor_data_to_controller_shared_data + read_index;

            printf("Valor atual lido pelo sensor: %d\n", tem_ptr->readed_value);
            
            if (tem_ptr->went_to_alarm_mode == 1) { // Se passou a modo alarme
                num_sensors_in_alarm++;
                printf("O valor de sensores em modo alarm aumentou! Existem atualmente %d sensores no modo alarm!\n", num_sensors_in_alarm);
            } else if (tem_ptr->stop_being_in_alarm_mode == 1) { // Se saiu de modo alarme
                num_sensors_in_alarm--;
                printf("O valor de sensores em modo alarm diminuiu! Existem atualmente %d sensores no modo alarm!\n", num_sensors_in_alarm);
            }

            // Limpar valor da memória partilhada para não haver problemas com futuras leituras
            tem_ptr->went_to_alarm_mode = 0;
            tem_ptr->stop_being_in_alarm_mode = 0;
            // Não é necessário limpar a leitura, pois será reescrita

            // Tratar índice de leitura da zona crítica
            read_index++;
            if (read_index == NUM_SENSORS)
                read_index = 0;

            //sem_post_with_validation(sem_excl);
            sem_post_with_validation(sem_space); // Informar que existe mais um espaço livre no buffer circular

            total_reads--;
        }

        // Sinalizar a remoção das memórias partilhadas
        shm_unlink_with_validations(SENSOR_DATA_TO_CONTROLLER_SHARED_MEMORY_NAME);
        shm_unlink_with_validations(WRITE_INDEX_SHARED_MEMORY_NAME);
        
        // Sinalizar a remoção dos semáforos criados.
        semaphore_unlink_with_validation(SEMAPHORE_SPACE);
        semaphore_unlink_with_validation(SEMAPHORE_EXCL);
        semaphore_unlink_with_validation(SEMAPHORE_DATA_TO_READ);

    } else { // Se for o processo filho (sensor)
        pid_t pid = getpid();
        srand(pid);

        sensor sens = {0};
        sens.sensor_id = i + 1;

        for (int i = 0; i < NUM_READS_BY_EACH_SENSOR; i++) {
            int actual_measurement = rand() % 101; // Número aleatório entre 0 e 100

            sens.last_read = sens.actual_read;
            sens.actual_read = actual_measurement;
            
            // Espera que haja espaço para escrever na zona crítica
            sem_wait_with_validation(sem_space);
            // Ganha acesso exclusívo à zona crítica
            sem_wait_with_validation(sem_excl);

            // Apontador para a zona da memória partilhada aonde os dados devem ser escritos
            sensor_data_to_controller *temp_ptr = sensor_data_to_controller_shared_data + *write_index;

            if (sens.is_in_alarm_mode == 0 && sens.actual_read >= 50) {
                sens.is_in_alarm_mode = 1;
                temp_ptr->went_to_alarm_mode = 1;
                printf("O sensor com o id %d está agora no modo alarm!\n", sens.sensor_id);
            } else if (sens.is_in_alarm_mode == 1 && sens.last_read < 50 && sens.actual_read < 50) {
                sens.is_in_alarm_mode = 0;
                temp_ptr->stop_being_in_alarm_mode = 1;
                printf("O sensor com o id %d deixou de estar no modo alarm!\n", sens.sensor_id);
            }

            temp_ptr->readed_value = sens.actual_read;

            // Tratar índice de escrita na zona crítica
            (*write_index)++;
            if (*write_index == NUM_SENSORS)
                *write_index = 0;

            // Libertar a zona crítica
            sem_post_with_validation(sem_excl);
            // Informar que existem novos dados a serem lidos da zona crítica
            sem_post_with_validation(sem_data_to_read);

            // Dormir durante 1 segundo
            sleep(1);
        }

    }

    // Fechar a zona crítica com os dados funcionais
    munmap_with_validations(sensor_data_to_controller_shared_data, sensor_data_to_controller_data_size);
    close_with_validations(fd_sensor_data_to_controller);

    munmap_with_validations(write_index, write_index_data_size);
    close_with_validations(fd_write_index);

    // Fechar os semáforos
    semaphore_close_with_validation(sem_space);
    semaphore_close_with_validation(sem_excl);
    semaphore_close_with_validation(sem_data_to_read);

}
