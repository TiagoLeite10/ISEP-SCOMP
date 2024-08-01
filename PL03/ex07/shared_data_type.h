#ifndef SHARED_DATA_TYPE_H
#define SHARED_DATA_TYPE_H
#define STR_SIZE 50
#define NR_DISC 10
    struct aluno {
        int numero;
        char nome[STR_SIZE];
        int disciplinas[NR_DISC];
        int dados_gravados;
    };
#endif