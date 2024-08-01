#define NOT_FOUND_VALUE -1

/* Função para procurar e encontrar a primeira ocorrência de um dado valor (value) num array com um dado tamanho (size) */
int find_first_occurrence(int *array, int size, int value) {

	int index = NOT_FOUND_VALUE;
	int i = 0;

	while (i < size && index == NOT_FOUND_VALUE) {
		if (*array == value) {
			index = i;
		}

		array++;
		i++;
	}

	return index;

}
