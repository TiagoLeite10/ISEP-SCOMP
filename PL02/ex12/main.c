#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#include "product.h"
#include "products.h"

#define READ 0
#define WRITE 1
#define NUMBER_OF_PRODUCTS 5
#define NUMBER_OF_BARCODES 6 // 6 para que seja testado quando é lido um barcode que não corresponde a nenhum produto
#define NUMBER_OF_BARCODE_READERS 5

volatile sig_atomic_t barcode_readed = 0;

void handler_USR1(int signo) {
	barcode_readed = 1;
}

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

// Estrutura que contém os campos necessários para o processo filho pedir ao processo pai informação acerca de um produto com um dado barcode
struct request_information {
	int pos_process;
	int barcode;
};

int main() {
	struct sigaction act;
	memset(&act, 0, sizeof(struct sigaction));
	sigfillset(&act.sa_mask);
	act.sa_handler = handler_USR1;
	sigaction(SIGUSR1, &act, NULL);

	printf("** Aviso: este programa irá correr em background! **\n");	

	int i;
	int fd_child_write_to_parent[2];
	int fd_parent_write_to_child[NUMBER_OF_BARCODE_READERS][2];

	pid_t p;
	pid_t parent_process = getpid();

	if (pipe(fd_child_write_to_parent) == -1) {
		perror("Pipe failed!\n");
		return 1;
	}

	for (i = 0; i < NUMBER_OF_BARCODE_READERS; i++) {
		if (pipe(fd_parent_write_to_child[i]) == -1) {
			perror("Pipe failed!\n");
			return 1;
		}

		p = fork();
		if (p == -1) {
			perror("Fork failed!\n");
			return 1;
		} else if (p == 0) {
			// Fecha no processo filho as ligações que o processo pai tem para escrita nos outros processos filho já criados
			for (int j = 0; j < i; j++) {
				close(fd_parent_write_to_child[j][WRITE]);
			}

			// Fecha no processo filho a escrita neste pipe, pois irá utilizar este pipe apenas para ler
			close(fd_parent_write_to_child[i][WRITE]);
			break;
		} else if (p > 0) {
			// Fecha no processo pai a leitura deste pipe, pois irá utilizar este pipe para escrever
			close(fd_parent_write_to_child[i][READ]);
		}
	}

	if (p > 0) {

		// Fecha no processo pai a escrita deste pipe, pois irá utilizar este pipe para ler
		close(fd_child_write_to_parent[WRITE]);

		Product products[NUMBER_OF_PRODUCTS];
		Product *products_ptr;
		products_ptr = products;

		fill_product_information(products_ptr);

		printf("Manual de utilização:\n");
		printf("Envie um sinal SIGUSR1 para um dos meus processos filhos para que estes gerem um barcode e comuniquem comigo de forma a que lhes dê informações acerca do produto com o barcode que é escolhido aleatóriamente!\n");

		while (1) {
			struct request_information ri = {0};
			size_t parent_read;
			size_t parent_write;

			// Solicitação do processo filho de informações acerca de um produto
			parent_read = read(fd_child_write_to_parent[READ], &ri, sizeof(ri));
			check_read(parent_read);

			// Encontra o índice do produto no array
			int product_index = find_product_index(products_ptr, ri.barcode, NUMBER_OF_PRODUCTS);

			// Saber se o barcode lido pertence a um produto existente
			int product_found = (product_index == -1 ? 0 : 1);

			// Informar o processo filho acerca se o produto existe ou não
			parent_write = write(fd_parent_write_to_child[ri.pos_process][WRITE], &product_found, sizeof(product_found));
			check_write(parent_write);

			if (product_found == 1) {
				Product *temp_product_ptr = products_ptr + product_index;
				Product product;

				product.barcode = temp_product_ptr->barcode;
				strcpy(product.name, temp_product_ptr->name);
				product.price = temp_product_ptr->price;
				
				// Envia as informações do produto com o barcode lido para o processo filho
				parent_write = write(fd_parent_write_to_child[ri.pos_process][WRITE], &product, sizeof(product));
				check_write(parent_write);
			}

		}

		close(fd_child_write_to_parent[READ]);
		for (int j = 0; j < NUMBER_OF_BARCODE_READERS; j++) {
			close(fd_parent_write_to_child[i][WRITE]);
		}

	} else {

		// Fecha no processo filho a leitura deste pipe, pois irá utilizar este pipe para escrever
		close(fd_child_write_to_parent[READ]);

		int available_barcodes[NUMBER_OF_BARCODES] = {11111, 22222, 33333, 44444, 55555, 66666};
		srand(getpid()); // Inicializa o gerador de números aleatórios
		size_t son_read;
		size_t son_write;
		
		while (1) {
			// Enquanto não receber o sinal que representa a leitura de um barcode (SIGUSR1)
			while (barcode_readed == 0) {
				pause(); // Espera por um sinal
			}

			// Se o PID do processo pai já não for o processo que criou este, terminar o processo
			if (parent_process != getppid()) {
				printf("Máquina em manutenção!\n");
				exit(1);
			}

			/* Gerar valor aleatório [0, NUMBER_OF_BARCODES[ */
			int sorted_number = rand() % NUMBER_OF_BARCODES;
			
			int barcode_to_check = available_barcodes[sorted_number];

			struct request_information ri;
			ri.barcode = barcode_to_check;
			ri.pos_process = i;

			printf("O barcode a verificar será o seguinte: %d\n", barcode_to_check);

			// Informa o processo pai do barcode lido
			son_write = write(fd_child_write_to_parent[WRITE], &ri, sizeof(ri));
			check_write(son_write);
			int valid_barcode = 0;

			// O processo pai informa este processo (filho) sobre se o produto com o barcode lido existe ou não
			son_read = read(fd_parent_write_to_child[i][READ], &valid_barcode, sizeof(valid_barcode));
			check_read(son_read);

			if (valid_barcode == 1) {
				Product product;

				// Lê as informações fornecidas pelo processo pai acerca do produto com o barcode lido
				son_read = read(fd_parent_write_to_child[i][READ], &product, sizeof(Product));
				check_read(son_read);

				printf("----- Informações do produto com o barcode %d -----\n", product.barcode);
				printf("Nome do produto: %s\n", product.name);
				printf("Preço do produto: %.2f€\n", product.price);

			} else {
				printf("Barcode não reconhecido!\n");
			}

			barcode_readed = 0;
		}

		close(fd_child_write_to_parent[WRITE]);
		close(fd_parent_write_to_child[i][READ]);

	}

	return 0;

}
