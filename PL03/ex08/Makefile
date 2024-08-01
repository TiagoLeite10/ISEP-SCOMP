# Makefile

# Variáveis
INCLUDES = count_word_in_file.h
SOURCES = count_word_in_file.c main.c
OBJFILES = count_word_in_file.o main.o
EXEC = ex08

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
