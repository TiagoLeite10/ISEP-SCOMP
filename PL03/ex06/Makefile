# Makefile

# Variáveis
INCLUDES = generate_random_values.h
SOURCES = generate_random_values.c main.c
OBJFILES = generate_random_values.o main.o
EXEC = ex06

# Regras para o sufixo
.SUFFIXES : .s .c .o

.s.o .c.o:
	gcc -Wall -g -c $<

${EXEC}: ${OBJFILES}
	gcc -Wall -g -o ${EXEC} ${OBJFILES} -lrt

${OBJFILES}: ${SOURCES} ${INCLUDES}

clean:
	rm -f ${OBJFILES} ${EXEC}

run: ${EXEC}
	./${EXEC}
