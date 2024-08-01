#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "generate_random_values.h"

#define ARRAY_SIZE 1000
#define LIMIT_NUMBER 10
#define NUMBER_CHILDREN 5
#define READ 0
#define WRITE 1

int main() {
	
	int fd[2];
	int i;
	int arr1[ARRAY_SIZE], arr2[ARRAY_SIZE];	

	pid_t p;

	/* Gerar valores aleatórios para os vetores 1 e 2 */
	generate_random_values(arr1, ARRAY_SIZE, LIMIT_NUMBER);
	generate_random_values(arr2, ARRAY_SIZE, LIMIT_NUMBER);

	int childs_index_range = ARRAY_SIZE / NUMBER_CHILDREN;

	if (pipe(fd) == -1) {
		perror("Pipe failed");
		return 1;
	}

	for (i = 0; i < NUMBER_CHILDREN; i++) {
		p = fork();
		if (p == 0)
			break;
		else if (p == -1) {
			perror("Fork failed");
			return 1;
		}

	}
	
	// Se for processo filho
	if (p == 0) {
		close(fd[READ]);
		
		int start_index = i * childs_index_range;
		int end_index = start_index + childs_index_range - 1;
		int tmp = 0;
		
		for (int j = start_index; j < end_index; j++) {
			tmp += arr1[j] + arr2[j];
		}

		write(fd[WRITE], &tmp, sizeof(tmp));

		close(fd[WRITE]);
	} else {
		close(fd[WRITE]);

		int tmp, result = 0;
	
		for (int j = 0; j < NUMBER_CHILDREN; j++) {
			read(fd[READ], &tmp, sizeof(tmp));
			result += tmp;
		}

		close(fd[READ]);

		printf("O resultado final é: %d\n", result);
	}

	return 0;
}
