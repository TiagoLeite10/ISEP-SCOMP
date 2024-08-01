# Makefile

# Variáveis
INCLUDES = generate_random_sales.h
SOURCES = generate_random_sales.c main.c
OBJFILES = generate_random_sales.o main.o
EXEC = ex09

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
