# Makefile

# Variáveis
INCLUDES = spawn_childs.h
SOURCES = spawn_childs.c main.c
OBJFILES = spawn_childs.o main.o
EXEC = ex12

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
