#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "simulate.h"

#define NUM_OF_WORKER_PROCESSES 50
#define MINIMUM_FINISHED_PROCESSES 25

volatile sig_atomic_t finished_processes = 0;
volatile sig_atomic_t successful_processes = 0;

void handle_USR1_and_USR2(int signo) {
	if (signo == SIGUSR1) {
		successful_processes++;
		finished_processes++;
	} else if (signo == SIGUSR2) {
		finished_processes++;
	}
}

void child_handle_USR1(int signo, siginfo_t *sinfo, void *context) {
	int simulation_number = getpid() - sinfo->si_pid;

	sleep(simulation_number);

	char msg_one[45];
    sprintf(msg_one, "(Simulation 2) Start of simulation %d.\n", simulation_number) ;
    write(STDOUT_FILENO, msg_one, strlen(msg_one));

	int success = simulate2();

	char msg_two[45];
    sprintf(msg_two, "(Simulation 2) End of simulation %d.\n", simulation_number) ;
    write(STDOUT_FILENO, msg_two, strlen(msg_two));

	kill(sinfo->si_pid, (success == 1 ? SIGUSR1 : SIGUSR2));
	exit(0);
}

int main() {

	pid_t p[NUM_OF_WORKER_PROCESSES];
	int is_child = 0;

	for (int i = 0; i < NUM_OF_WORKER_PROCESSES; i++) {
		p[i] = fork();

		if (p[i] == -1) {
			perror("Fork failed!\n");
			return 1;
		} else if (p[i] == 0) {
			is_child = 1;
			break;
		}
	}

	// se for o processo filho
	if (is_child == 1) {

		struct sigaction child_act_usr1;
		memset(&child_act_usr1, 0, sizeof(struct sigaction));
		sigfillset(&child_act_usr1.sa_mask);
		child_act_usr1.sa_sigaction = child_handle_USR1;
		child_act_usr1.sa_flags = SA_SIGINFO;
		sigaction(SIGUSR1, &child_act_usr1, NULL);
		
		int simulation_number = getpid() - getppid();

		sleep(simulation_number);

		printf("(Simulation 1) Start of simulation %d.\n", simulation_number);

		int has_relevant_data = simulate1();
		kill(getppid(), (has_relevant_data == 1 ? SIGUSR1 : SIGUSR2));
		
		printf("(Simulation 1) End of simulation %d.\n", simulation_number);

		pause();
		
	} else {

		struct sigaction act;
		memset(&act, 0, sizeof(struct sigaction));
		sigfillset(&act.sa_mask);
		act.sa_handler = handle_USR1_and_USR2;
		sigaction(SIGUSR1, &act, NULL);
		sigaction(SIGUSR2, &act, NULL);
		
		while(finished_processes < MINIMUM_FINISHED_PROCESSES) {
			pause();
		}

		if (successful_processes == 0) {
			printf("Inefficient algorithm!\n");

			for (int j = 0; j < NUM_OF_WORKER_PROCESSES; j++) {
				kill(p[j], SIGKILL);
			}
		} else {
			printf("\n-- Simulate 1 results --\n");
			printf("Number of terminated processes: %d\n", finished_processes);
			printf("Number of successful processes: %d\n\n", successful_processes);

			successful_processes = 0;
			finished_processes = 0;

			for (int j = 0; j < NUM_OF_WORKER_PROCESSES; j++) {
				kill(p[j], SIGUSR1);
			}

			while (finished_processes < NUM_OF_WORKER_PROCESSES) {
				pause();
			}

			printf("\n-- Simulate 2 results --\n");
			printf("Number of terminated processes: %d\n", finished_processes);
			printf("Number of successful processes: %d\n\n", successful_processes);

		}
	}

	return 0;
}
