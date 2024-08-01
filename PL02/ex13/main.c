#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUMBER_MACHINE 4
#define NUMBER_PIECES 1000
#define PIECES_M1_M2 5
#define PIECES_M2_M3 5
#define PIECES_M3_M4 10
#define PIECES_M4_A1 100
#define READ 0
#define WRITE 1

int main() {	
	int fd[NUMBER_MACHINE][2], fd_warning[NUMBER_MACHINE][2];
	int pieces_passed[NUMBER_MACHINE] = {PIECES_M1_M2, PIECES_M2_M3, PIECES_M3_M4, PIECES_M4_A1};
	int i, production = 0, inventory = 0, pieces_received = 0, pieces_send = 0;
	int warning_send = 1, warning_finish = 0, warning_received;
	pid_t p;

	for (i = 0; i < NUMBER_MACHINE; i++) {
		
		/* Cria os pipes da máquina */
		if (pipe(fd[i]) == -1) {
			perror("Falha no pipe da máquina");
			exit(EXIT_FAILURE);
		}

		/* Cria os pipes de aviso */
		if (pipe(fd_warning[i]) == -1) {
			perror("Falha no pipe de aviso");
			exit(EXIT_FAILURE);
		}

		p = fork();

		/* Caso aconteça uma falha na criação do processo filho */
		if (p == -1) {
			perror("Fork falhou");
			exit(EXIT_FAILURE);
		}

		if (p == 0) {

			/* Fecha as extremidades não usadas */
			close(fd[i][READ]);
			close(fd_warning[i][READ]);
			
			if (i == 0) {

				/* Inicia o processamento das peças até finalizar*/
				while (NUMBER_PIECES > production) {
					/* Escreve no pipe da máquina */
					write(fd[i][WRITE], &pieces_passed[i], sizeof(pieces_passed[i]));
					write(fd_warning[i][WRITE], &warning_send, sizeof(warning_send));
					
					production += pieces_passed[i];

					printf("Enviado %d peças da M%d para a M%d!\n", pieces_passed[i], i + 1, i + 2);
				}

				write(fd_warning[i][WRITE], &warning_finish, sizeof(warning_finish));	

			} else {

				read(fd_warning[i - 1][READ], &warning_received, sizeof(warning_received));

				/* Conforme a receção dos avisos vai ser passadas as peças para a máquina seguinte */
				while (warning_received != 0) {

					if (warning_received == 1) {						
						read(fd[i - 1][READ], &pieces_received, sizeof(pieces_received));
						pieces_send += pieces_received;

						if (pieces_send == pieces_passed[i]) {
							/* Escreve no pipe da máquina */
							write(fd[i][WRITE], &pieces_passed[i], sizeof(pieces_passed[i]));
							write(fd_warning[i][WRITE], &warning_send, sizeof(warning_send));

							if (i == NUMBER_MACHINE - 1) {
								printf("Enviado %d peças da M%d para o depósito A1!\n", pieces_passed[i], i + 1);
							} else {
								printf("Enviado %d peças da M%d para a M%d!\n", pieces_passed[i], i + 1, i + 2);
							}
							
							pieces_send = 0;
						}
					}

					read(fd_warning[i - 1][READ], &warning_received, sizeof(warning_received));
				}	

				write(fd_warning[i][WRITE], &warning_finish, sizeof(warning_finish));				
			}

			/* Fecha as extremidades usadas */
			close(fd[i][READ]);
			close(fd_warning[i][READ]);

			exit(EXIT_SUCCESS);			
		}
	}

	int last_machine_number = NUMBER_MACHINE - 1;

	read(fd_warning[last_machine_number][READ], &warning_received, sizeof(warning_received));

	/* Conforme a receção dos avisos vai ser adicionadas as peças ao inventário final */
	while (warning_received != 0) {
		if (warning_received == 1) {						
			read(fd[last_machine_number][READ], &pieces_received, sizeof(pieces_received));

			if (pieces_received == pieces_passed[last_machine_number]) {
				inventory += pieces_received;
			}
		}

		read(fd_warning[last_machine_number][READ], &warning_received, sizeof(warning_received));
	}

	printf("Foram adicionadas %d peças ao inventário!\n", inventory);

	/* Fecha as extremidades usadas */
	close(fd[last_machine_number][READ]);
	close(fd_warning[last_machine_number][READ]);

	return 0;
}