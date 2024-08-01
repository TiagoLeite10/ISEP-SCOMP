# Makefile

# Variáveis
INCLUDES = generate_random_values.h find_first_occurrence.h
SOURCES = generate_random_values.c find_first_occurrence.c main.c
OBJFILES = generate_random_values.o find_first_occurrence.o main.o
EXEC = ex10

# Regras para o sufixo
.SUFFIXES : .s .c .o

.s.o .c.o:
	gcc -Wall -g -c $<

${EXEC}: ${OBJFILES}
	gcc -Wall -g -o ${EXEC} ${OBJFILES}

${OBJFILES}: ${SOURCES} ${INCLUDES}

clean:
	rm -f ${OBJFILES} ${EXEC}

run: ${EXEC}
	./${EXEC}
