#include <stdio.h>
#include <stdlib.h>

/*
* Método para contar o número de vezes que uma dada palavra aparece num dado ficheiro.
* path_to_file é o caminho para o ficheiro.
* word_to_find é a palavra a encontrar no ficheiro.
*/
int count_word_in_file(char *path_to_file, char *word_to_find) {

    int count = 0;
    
    FILE *fptr;
    fptr = fopen(path_to_file, "r");

    if (fptr == NULL) {
        printf("A error occurred while trying to open the file.");
        exit(1);
    }

    char c;
    char * word_to_find_temp;
    word_to_find_temp = word_to_find;

    while (1) {
        c = fgetc(fptr);
        
        // Se o caracter atual do ficheiro for igual ao caracter atual da palavra
        if (c == *word_to_find_temp) {
            word_to_find_temp++;
        } else if (*word_to_find_temp == '\0') { // Se a palavra já foi toda lida
            count++;
            word_to_find_temp = word_to_find;
        } else if (c != *word_to_find_temp) { // Se o caracter atual do ficheiro for diferente ao caracter atual da palavra
            word_to_find_temp = word_to_find;
        }

        if (feof(fptr)) {
            break;
        }

    }
    
    fclose(fptr);

    return count;

}