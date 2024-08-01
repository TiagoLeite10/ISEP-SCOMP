#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define READ 0
#define WRITE 1
#define START_CREDIT 5
#define LIMIT_RAND_NUMBER 5
#define CAN_BET 1
#define CREDIT_ENDED 0
#define WIN_QUANTITY 10
#define LOSE_QUANTITY 5

/* Valida se foi possível ler do pipe com sucesso */
void check_read(size_t value) {
	if (value == -1) {
		perror("Something went wrong while trying to read from pipe!\n");
		exit(1);
	}
}

/* Valida se foi possível escrever no pipe com sucesso */
void check_write(size_t value) {
	if (value == -1) {
		perror("Something went wrong while trying to write in the pipe!\n");
		exit(1);
	}
}

int main() {	
	int fd_parent_to_child[2];
	int fd_child_to_parent[2];

	if (pipe(fd_parent_to_child) == -1 || pipe(fd_child_to_parent) == -1) {
		perror("Pipe failed!\n");
		return 1;
	}

	pid_t p = fork();
	if (p == -1) {
		perror("Fork failed!\n");
		return 1;
	}

	// Se for o pai
	if (p > 0) {
		srand(getpid()); // Inicializa o gerador de números aleatórios

		int credit = START_CREDIT;
		
		close(fd_parent_to_child[READ]);
		close(fd_child_to_parent[WRITE]);

		while (credit > 0) {
			size_t parent_write_size = 0;
			size_t parent_read_size = 0;
			
			// Se o crédito for maior do que 0 euros, é possível apostar
			int is_bet_available = credit > 0 ? CAN_BET : CREDIT_ENDED;

			// Informa o filho se é possível ou não apostar
			parent_write_size = write(fd_parent_to_child[WRITE], &is_bet_available, sizeof(int));
			check_write(parent_write_size);

			int child_selected_number = 0;
			
			// Lê o valor que o filho escolhe para entrar na aposta, ou o caso em que ele não pode apostar (o processo filho termina e o read devolve o valor 0)
			parent_read_size = read(fd_child_to_parent[READ], &child_selected_number, sizeof(child_selected_number));
			check_read(parent_read_size);
			if (parent_read_size == 0) {
				credit = 0; // Se o processo filho terminar por alguma razão, "forçar" o término do processo pai
			} else {
				/* Gerar valor aleatório entre 1 e LIMIT_RAND_NUMBER */
				int sorted_number = rand() % LIMIT_RAND_NUMBER + 1;

				// Se o valor escolhido pelo processo filho for igual ao valor gerado pelo processo pai
				if (child_selected_number == sorted_number) {
					credit += WIN_QUANTITY;
					printf("Congratulations! You just won %d euros!\n", WIN_QUANTITY);
				} else {
					credit -= LOSE_QUANTITY;
					printf("What a bad luck! You just lose %d euros!\n", LOSE_QUANTITY);
				}

				// Informar o processo filho do seu crédito atual
				parent_write_size = write(fd_parent_to_child[WRITE], &credit, sizeof(credit));
				check_write(parent_write_size);
			}
			
		}

		close(fd_child_to_parent[READ]);
		close(fd_parent_to_child[WRITE]);

	} else {
		srand(getpid()); // Inicializa o gerador de números aleatórios

		close(fd_child_to_parent[READ]);
		close(fd_parent_to_child[WRITE]);
		int can_bet = CAN_BET;

		while (can_bet == CAN_BET) {
			// Este processo é informado pelo processo pai se pode ou não apostar (se tem dinheiro para o fazer)
			size_t child_read_size = read(fd_parent_to_child[READ], &can_bet, sizeof(can_bet));
			check_read(child_read_size);

			if (can_bet == CAN_BET) {
				/* Gerar valor aleatório entre 1 e LIMIT_RAND_NUMBER */
				int selected_number = rand() % LIMIT_RAND_NUMBER + 1;

				// Informa o processo pai do valor em que deseja apostar
				size_t child_write_size = write(fd_child_to_parent[WRITE], &selected_number, sizeof(selected_number));
				check_write(child_write_size);
				
				int actual_credit = 0;
				// Recebe a informação do processo pai acerca do seu crédito atual
				child_read_size = read(fd_parent_to_child[READ], &actual_credit, sizeof(actual_credit));
				check_read(child_read_size);

				printf("The actual credit is %d euros!\n", actual_credit);
			}
		}

		close(fd_parent_to_child[READ]);
		close(fd_child_to_parent[WRITE]);
	}

	return 0;

}
