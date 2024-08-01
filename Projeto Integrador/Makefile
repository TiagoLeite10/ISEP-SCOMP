# Makefile

# Variáveis
SOURCES_LIBS_SHM = libs/my_shared_memory_lib.c
OBJFILES_LIBS_SHM = libs/my_shared_memory_lib.o

SOURCES_LIBS_SEM = libs/my_semaphore_lib.c
OBJFILES_LIBS_SEM = libs/my_semaphore_lib.o

INCLUDES = libs/my_shared_memory_lib.h libs/my_semaphore_lib.h
SOURCES = libs/my_shared_memory_lib.c libs/my_semaphore_lib.c main.c
OBJFILES = libs/my_shared_memory_lib.o libs/my_semaphore_lib.o main.o
EXEC = scomp

# Regras para o sufixo
.SUFFIXES : .s .c .o

.s.o .c.o:
	gcc -Wall -g -c $<

${EXEC}: ${OBJFILES}
	gcc -Wall -g -o ${EXEC} ${OBJFILES} -lrt -lpthread

# Regra explícita para gerar my_shared_memory_lib.o
${OBJFILES_LIBS_SHM}: ${SOURCES_LIBS_SHM}
	gcc -Wall -g -c $< -o $@

# Regra explícita para gerar my_semaphore_lib.o
${OBJFILES_LIBS_SEM}: ${SOURCES_LIBS_SEM}
	gcc -Wall -g -c $< -o $@

${OBJFILES}: ${SOURCES} ${INCLUDES}

clean:
	rm -f ${OBJFILES} ${EXEC}

run: ${EXEC}
	./${EXEC}
