#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define READ 0
#define WRITE 1
#define NUM_CHILD_PROCESSES 10
#define SLEEP_TIME 2

struct win_the_pipe {
	char msg[4];
	int round;
};

int main() {	
	
	int fd[2];
	int is_child = 0;
	pid_t p[NUM_CHILD_PROCESSES];

	if (pipe(fd) == -1) {
		perror("Pipe failed!\n");
		return 1;
	}

	for (int i = 0; i < NUM_CHILD_PROCESSES; i++) {
		p[i] = fork();
		if (p[i] == 0) {
			is_child = 1;
			break;
		} else if (p[i] == -1) {
			perror("Fork failed!\n");
			return 1;
		}
	}

	// Se for o pai
	if (is_child == 0) {
		
		close(fd[READ]);

		struct win_the_pipe wtp;
		
		strcpy(wtp.msg, "Win");

		for (int i = 0; i < NUM_CHILD_PROCESSES; i++) {
			sleep(SLEEP_TIME);
			wtp.round = i + 1;
			write(fd[WRITE], &wtp, sizeof(struct win_the_pipe));
		}

		close(fd[WRITE]);

		for (int i = 0; i < NUM_CHILD_PROCESSES; i++) {
			int status;
			waitpid(p[i], &status, 0);
			printf("The process with the PID %d, won the %dº\n", p[i], WEXITSTATUS(status));
		}

	} else {
		
		close(fd[WRITE]);

		struct win_the_pipe winner;

		read(fd[READ], &winner, sizeof(struct win_the_pipe));

		printf("%s the round %d!\n", winner.msg, winner.round);

		close(fd[READ]);

		exit(winner.round);

	}

	return 0;

}
