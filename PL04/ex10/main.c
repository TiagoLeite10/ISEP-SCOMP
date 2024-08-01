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
#include <signal.h>

#include "../libs/my_shared_memory_lib.h"
#include "../libs/my_semaphore_lib.h"
#include "metro_data.h"
#include "passenger_data.h"

#define SHARED_MEMORY_NAME "/ex10"
#define SEM_SPACE_NAME "/ex10_sem_space"
#define SEM_N_PROC "/ex10_sem_n_proc"
#define SEM_PASSENGER_ENTERED_IN_METRO "/ex10_sem_passenger_entered_in_metro"
#define SEM_DOOR1_NAME "/ex10_sem_door_1"
#define SEM_DOOR2_NAME "/ex10_sem_door_2"
#define SEM_DOOR3_NAME "/ex10_sem_door_3"

#define NUMBER_PASSANGERS_ENTER_BEFORE_STATION_A 15
#define NUMBER_PASSANGERS_ENTER_IN_STATION_A 20
#define NUMBER_PASSANGERS_ENTER_IN_STATION_B 5

#define NUMBER_PASSENGERS_LEAVE_IN_STATION_A 5

#define MAX_TIME_TO_WAIT_TO_ENTER_METRO 3
#define MAX_TIME_TO_WAIT_FOR_PASSENGER 10

#define ERROR_VALUE -1

#define TRIP_TIME 5
#define MISSING_METRO_TIME 10

#define TIME_TO_PASS_DOOR 0.2

void passenger(int fd, off_t data_size, metro_data *shared_data, sem_t *sem_doors[NUMBER_OF_DOORS], passenger_data m_passenger, sem_t *sem_space_in_metro, sem_t *sem_passenger_entered_in_metro, sem_t *sem_n_proc);

void inform_passengers_arrived_new_station(metro_data *shared_data) {
	for (int i = 0; i < MAXIMUM_CAPACITY_OF_THE_METRO; i++) {
		if (shared_data->pid_passengers[i] != 0) {
			sem_post_with_validation(shared_data->passangers_position[i]);
		}
	}
}

void wait_passengers_to_enter(metro_data *shared_data, sem_t *sem_passenger_entered_in_metro, sem_t *sem_n_proc, int num_new_passangers) {
	int can_go = 0;
	struct timespec timeout_waiting;
	while (!can_go) {
		// Calcula o intervalo relativo com a hora atual mais o número máximo de segundos.
		if (clock_gettime(CLOCK_REALTIME, &timeout_waiting) == -1) {
			perror("Something went wrong while doing the clock_gettime!\n");
			exit(EXIT_FAILURE);
		}

		timeout_waiting.tv_sec += MAX_TIME_TO_WAIT_FOR_PASSENGER;

		// Espera no máximo MAX_TIME_TO_WAIT_FOR_A_CLIENT para decrementar o semáforo. (espera que clientes entrem na fila de espera)
		int timeout_result = sem_timedwait(sem_passenger_entered_in_metro, &timeout_waiting);
		
		if (timeout_result == -1) {
			// Se o tempo esgotar
			if (errno == ETIMEDOUT) {
				printf("Os passangeiros foram muito lentos a entrar/sair no metro! :(\n");
				exit(EXIT_FAILURE);
			} else { // Se der erro na função
				perror("Something went wrong while doing the sem_timedwait()!\n");
				exit(EXIT_FAILURE);
			}
		} else {
			sem_wait_with_validation(sem_n_proc);

			if (shared_data->num_new_passangers == num_new_passangers) 
				can_go = 1;

			sem_post_with_validation(sem_n_proc);

		}
	}
}

int main() {

	int fd;
	off_t data_size = sizeof(metro_data);
	metro_data *shared_data;

	fd = shm_open_with_validations(SHARED_MEMORY_NAME, O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
	ftruncate_with_validations(fd, data_size);
	shared_data = (metro_data *) mmap_with_validations(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	shared_data->write_index = 0;

	sem_t *sem_space_in_metro = semaphore_open_with_validation(SEM_SPACE_NAME, O_CREAT, 0644, MAXIMUM_CAPACITY_OF_THE_METRO);

	sem_t *sem_n_proc = semaphore_open_with_validation(SEM_N_PROC, O_CREAT, 0644, 1);

	sem_t *sem_passenger_entered_in_metro = semaphore_open_with_validation(SEM_PASSENGER_ENTERED_IN_METRO, O_CREAT, 0644, 0);


	sem_t *sem_doors[NUMBER_OF_DOORS];
	sem_doors[DOOR_1] = semaphore_open_with_validation(SEM_DOOR1_NAME, O_CREAT, 0644, 1);
    sem_doors[DOOR_2] = semaphore_open_with_validation(SEM_DOOR2_NAME, O_CREAT, 0644, 1);
    sem_doors[DOOR_3] = semaphore_open_with_validation(SEM_DOOR3_NAME, O_CREAT, 0644, 1);

	// Cria os semáforos na memória partilhada, que serão usados para indicar que um cliente numa dada posição já pode entrar
    for (int i = 0; i < MAXIMUM_CAPACITY_OF_THE_METRO; i++) {
        char sem_name[33];
        sprintf(sem_name, "/ex10_sempahore_passenger_%d", i);
        shared_data->passangers_position[i] = semaphore_open_with_validation(sem_name, O_CREAT, 0644, 0);
    }

	shared_data->actual_station = STATION_OTHER;

	printf("\n**METRO INFORMATION:** Vou carregar passageiros antes de ir para a estação A!\n\n");

	pid_t p;
	int i;
	passenger_data m_passenger[NUMBER_PASSANGERS_ENTER_BEFORE_STATION_A] = {0};
	// Antes da estação A
	for (i = 0; i < NUMBER_PASSANGERS_ENTER_BEFORE_STATION_A; i++) { // Criar os processos filho
        p = fork();
        /* Caso aconteça uma falha na criação do processo filho */
        if (p == ERROR_VALUE) {
            perror("Fork failed!\n");
            exit(EXIT_FAILURE);
        }

        if (p == 0) { // Se for processo filho
			m_passenger[i].enter_station = STATION_OTHER;
			strcpy(m_passenger[i].enter_station_description, "desconhecida");

			if (i < NUMBER_PASSENGERS_LEAVE_IN_STATION_A) {
				m_passenger[i].leave_station = STATION_A;
				strcpy(m_passenger[i].leave_station_description, "A");
			} else {
				m_passenger[i].leave_station = STATION_B;
				strcpy(m_passenger[i].leave_station_description, "B");
			}

			passenger(fd, data_size, shared_data, sem_doors, m_passenger[i], sem_space_in_metro, sem_passenger_entered_in_metro, sem_n_proc);

		} 

    }

	wait_passengers_to_enter(shared_data, sem_passenger_entered_in_metro, sem_n_proc, 15);
	
	printf("\n**METRO INFORMATION:** Vou agora para a estação A com %d passageiros!\n\n", shared_data->passengers_inside_metro);
	sleep(TRIP_TIME);
	shared_data->actual_station = STATION_A;
	printf("\n**METRO INFORMATION:** Cheguei à estação A e estou à espera que entrem passegeiros!\n\n");
	shared_data->num_new_passangers = 0;
	inform_passengers_arrived_new_station(shared_data);

	passenger_data a_passenger[NUMBER_PASSANGERS_ENTER_IN_STATION_A];
	// Estação A
	for (i = 0; i < NUMBER_PASSANGERS_ENTER_IN_STATION_A; i++) { // Criar os processos filho
        p = fork();
        /* Caso aconteça uma falha na criação do processo filho */
        if (p == ERROR_VALUE) {
            perror("Fork failed!\n");
            exit(EXIT_FAILURE);
        }

        if (p == 0) { // Se for processo filho
			a_passenger[i].leave_station = STATION_OTHER;
			strcpy(a_passenger[i].leave_station_description, "desconhecida");
			a_passenger[i].enter_station = STATION_A;
			strcpy(a_passenger[i].enter_station_description, "A");
            break;
		} 

    }

	if (p == 0)
		passenger(fd, data_size, shared_data, sem_doors, a_passenger[i], sem_space_in_metro, sem_passenger_entered_in_metro, sem_n_proc);

	wait_passengers_to_enter(shared_data, sem_passenger_entered_in_metro, sem_n_proc, 10);
	printf("\n**METRO INFORMATION:** Vou sair agora da estação A com %d passageiros!\n\n", shared_data->passengers_inside_metro);
	sleep(TRIP_TIME);
	shared_data->actual_station = STATION_B;
	printf("\n**METRO INFORMATION:** Cheguei à estação B e estou à espera que entrem passegeiros!\n\n");

	shared_data->num_new_passangers = 0;
	inform_passengers_arrived_new_station(shared_data);

	passenger_data b_passenger[NUMBER_PASSANGERS_ENTER_IN_STATION_B];
	// Estação B
	for (i = 0; i < NUMBER_PASSANGERS_ENTER_IN_STATION_B; i++) { // Criar os processos filho
        p = fork();
        /* Caso aconteça uma falha na criação do processo filho */
        if (p == ERROR_VALUE) {
            perror("Fork failed!\n");
            exit(EXIT_FAILURE);
        }

        if (p == 0) { // Se for processo filho
			b_passenger[i].leave_station = STATION_OTHER;
			strcpy(b_passenger[i].leave_station_description, "desconhecida");
			b_passenger[i].enter_station = STATION_B;
			strcpy(b_passenger[i].enter_station_description, "B");
            break;
		} 

    }

	if (p == 0)
		passenger(fd, data_size, shared_data, sem_doors, b_passenger[i], sem_space_in_metro, sem_passenger_entered_in_metro, sem_n_proc);

	shared_data->num_new_passangers = 0;
	wait_passengers_to_enter(shared_data, sem_passenger_entered_in_metro, sem_n_proc, 5);
	
	printf("\n**METRO INFORMATION:** Vou sair agora da estação B com %d passageiros!\n\n", shared_data->passengers_inside_metro);
	sleep(MISSING_METRO_TIME);

	shared_data->actual_station = STATION_OTHER;
	shared_data->num_new_passangers = 0;
	inform_passengers_arrived_new_station(shared_data);
	

	printf("\n**NEWS INFORMATION:** E o metro nunca mais foi visto, talvez volte numa manhã de nevoeiro!\n\n");

	// Sinaliza a remoção da zona crítica
	shm_unlink_with_validations(SHARED_MEMORY_NAME);

	// Sinaliza a remoção dos semáforos
	semaphore_unlink_with_validation(SEM_SPACE_NAME);
	semaphore_unlink_with_validation(SEM_N_PROC);
	semaphore_unlink_with_validation(SEM_PASSENGER_ENTERED_IN_METRO);
	semaphore_unlink_with_validation(SEM_DOOR1_NAME);
	semaphore_unlink_with_validation(SEM_DOOR2_NAME);
	semaphore_unlink_with_validation(SEM_DOOR3_NAME);

	for (int i = 0; i < MAXIMUM_CAPACITY_OF_THE_METRO; i++) {
		char sem_name[33];
		sprintf(sem_name, "/ex10_sempahore_passenger_%d", i);
		semaphore_unlink_with_validation(sem_name);
	}

	// Fecha os semáforos
	semaphore_close_with_validation(sem_space_in_metro);
	semaphore_close_with_validation(sem_n_proc);
	semaphore_close_with_validation(sem_passenger_entered_in_metro);
	semaphore_close_with_validation(sem_doors[DOOR_1]);
	semaphore_close_with_validation(sem_doors[DOOR_2]);
	semaphore_close_with_validation(sem_doors[DOOR_3]);

	for (int i = 0; i < MAXIMUM_CAPACITY_OF_THE_METRO; i++) {
        semaphore_close_with_validation(shared_data->passangers_position[i]);
    }

	// Fecha a zona de memória partilhada
	munmap_with_validations(shared_data, data_size);
	close_with_validations(fd);

	return 0;
}

void passenger(int fd, off_t data_size, metro_data *shared_data, sem_t *sem_doors[NUMBER_OF_DOORS], passenger_data m_passenger, sem_t *sem_space_in_metro, sem_t *sem_passenger_entered_in_metro, sem_t *sem_n_proc) {
    pid_t pid = getpid();
    srand(pid);

    switch (rand() % 3 + 1) {
    case 1:
        m_passenger.choosen_door = DOOR_1;
        break;
    case 2:
        m_passenger.choosen_door = DOOR_2;
        break;
    default:
        m_passenger.choosen_door = DOOR_3;
        break;
    }

	struct timespec timeout_waiting;
	// Calcula o intervalo relativo com a hora atual mais o número máximo de segundos.
	if (clock_gettime(CLOCK_REALTIME, &timeout_waiting) == -1) {
		perror("Something went wrong while doing the clock_gettime!\n");
		exit(EXIT_FAILURE);
	}

	timeout_waiting.tv_sec += MAX_TIME_TO_WAIT_TO_ENTER_METRO;

	printf("Estou à espera do metro na estação %s para ir para a estação %s! Quero entrar pela porta número %d! (pid %d)\n", m_passenger.enter_station_description, m_passenger.leave_station_description, m_passenger.choosen_door, pid);

	// Espera no máximo MAX_TIME_TO_WAIT_FOR_A_CLIENT para decrementar o semáforo. (espera que clientes entrem na fila de espera)
	int timeout_result = sem_timedwait(sem_space_in_metro, &timeout_waiting);
	
	if (timeout_result == -1) {
		// Se o tempo esgotar
		if (errno == ETIMEDOUT) {
			printf("Vou embora, não consegui entrar no metro! (Estava na estação %s) :( (pid %d)\n", m_passenger.enter_station_description, pid);
		} else { // Se der erro na função
			perror("Something went wrong while doing the sem_timedwait()!\n");
			exit(EXIT_FAILURE);
		}
	} else {
		sem_wait_with_validation(sem_doors[m_passenger.choosen_door]);
		sleep(TIME_TO_PASS_DOOR);
		sem_post_with_validation(sem_doors[m_passenger.choosen_door]);

		int pos;
		sem_wait_with_validation(sem_n_proc);
		shared_data->num_new_passangers++;
		shared_data->passengers_inside_metro++;
		pos = shared_data->write_index;
		while (shared_data->pid_passengers[pos] != 0) {
			pos++;
			if (pos == MAXIMUM_CAPACITY_OF_THE_METRO) pos = 0;
		}

		shared_data->write_index = pos;
		shared_data->pid_passengers[pos] = pid;

		sem_post_with_validation(sem_n_proc);
		
		sem_post_with_validation(sem_passenger_entered_in_metro);

		printf("Já estou dentro do metro! (%d)\n", pid);
		
		while (1) {
			sem_wait_with_validation(shared_data->passangers_position[pos]);
			if (shared_data->actual_station == m_passenger.leave_station)
				break;
			
		}
		
		sem_wait_with_validation(sem_doors[m_passenger.choosen_door]);
		sleep(TIME_TO_PASS_DOOR);
		sem_post_with_validation(sem_doors[m_passenger.choosen_door]);

		sem_post_with_validation(sem_space_in_metro);
		sem_wait_with_validation(sem_n_proc);
		shared_data->passengers_inside_metro--;
		shared_data->pid_passengers[pos] = 0;
		sem_post_with_validation(sem_n_proc);

		if (m_passenger.leave_station != STATION_OTHER)
			printf("Sai agora do metro pela porta %d na estação %s! Vim da estação %s! (pid %d)\n", m_passenger.choosen_door, m_passenger.leave_station_description, m_passenger.enter_station_description, pid);

	}

	// Fecha os semáforos
	semaphore_close_with_validation(sem_space_in_metro);
	semaphore_close_with_validation(sem_n_proc);
	semaphore_close_with_validation(sem_passenger_entered_in_metro);
	semaphore_close_with_validation(sem_doors[DOOR_1]);
	semaphore_close_with_validation(sem_doors[DOOR_2]);
	semaphore_close_with_validation(sem_doors[DOOR_3]);

	for (int i = 0; i < MAXIMUM_CAPACITY_OF_THE_METRO; i++) {
        semaphore_close_with_validation(shared_data->passangers_position[i]);
    }

	// Fecha a zona de memória partilhada
	munmap_with_validations(shared_data, data_size);
	close_with_validations(fd);

	exit(EXIT_SUCCESS);
}
