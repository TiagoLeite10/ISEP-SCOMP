# Makefile

# Variáveis
SOURCES_LIBS_SEM = ../libs/my_semaphore_lib.c
OBJFILES_LIBS_SEM = ../libs/my_semaphore_lib.o

INCLUDES = ../libs/my_semaphore_lib.h
SOURCES = ../libs/my_semaphore_lib.c main.c
OBJFILES = ../libs/my_semaphore_lib.o main.o
EXEC = ex06

# Regras para o sufixo
.SUFFIXES : .s .c .o

.s.o .c.o:
	gcc -Wall -g -c $<

${EXEC}: ${OBJFILES}
	gcc -Wall -g -o ${EXEC} ${OBJFILES} -lpthread

# Regra explícita para gerar my_semaphore_lib.o
${OBJFILES_LIBS_SEM}: ${SOURCES_LIBS_SEM}
	gcc -Wall -g -c $< -o $@

${OBJFILES}: ${SOURCES} ${INCLUDES}

clean:
	rm -f ${OBJFILES} ${EXEC}

run: ${EXEC}
	./${EXEC}
