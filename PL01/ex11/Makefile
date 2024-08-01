# Makefile

# Variáveis
INCLUDES = generate_random_values.h search_max_value.h perform_calculation.h
SOURCES = generate_random_values.c search_max_value.c perform_calculation.c main.c
OBJFILES = generate_random_values.o search_max_value.o perform_calculation.o main.o
EXEC = ex11

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
