# Makefile

# Variáveis
SOURCES_LIBS_SHM = ../../libs/my_shared_memory_lib.c
OBJFILES_LIBS_SHM = ../../libs/my_shared_memory_lib.o

SOURCES_LIBS_SEM = ../../libs/my_semaphore_lib.c
OBJFILES_LIBS_SEM = ../../libs/my_semaphore_lib.o

INCLUDES_WRITER = ../../libs/my_shared_memory_lib.h ../../libs/my_semaphore_lib.h
SOURCES_WRITER = ../../libs/my_shared_memory_lib.c ../../libs/my_semaphore_lib.c writer.c
OBJFILES_WRITER = ../../libs/my_shared_memory_lib.o ../../libs/my_semaphore_lib.o writer.o
EXEC_WRITER = writer

INCLUDES_READER = ../../libs/my_shared_memory_lib.h ../../libs/my_semaphore_lib.h
SOURCES_READER = ../../libs/my_shared_memory_lib.c ../../libs/my_semaphore_lib.c reader.c
OBJFILES_READER = ../../libs/my_shared_memory_lib.o ../../libs/my_semaphore_lib.o reader.o
EXEC_READER = reader

INCLUDES_READER = ../../libs/my_shared_memory_lib.h ../../libs/my_semaphore_lib.h
SOURCES = ../../libs/my_shared_memory_lib.c ../../libs/my_semaphore_lib.c main.c
OBJFILES = ../../libs/my_shared_memory_lib.o ../../libs/my_semaphore_lib.o main.o
EXEC = ex13_writers_with_priority

# Regras para o sufixo
.SUFFIXES : .s .c .o

.s.o .c.o:
	gcc -Wall -g -c $<

${EXEC_WRITER}: ${OBJFILES_WRITER}
	gcc -Wall -g -o ${EXEC_WRITER} ${OBJFILES_WRITER} -lrt -lpthread

${OBJFILES_WRITER}: ${SOURCES_WRITER} ${INCLUDES_WRITER} 

${EXEC_READER}: ${OBJFILES_READER}
	gcc -Wall -g -o ${EXEC_READER} ${OBJFILES_READER} -lrt -lpthread

${OBJFILES_READER}: ${SOURCES_READER} ${INCLUDES_READER}

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
	rm -f ${OBJFILES_WRITER} ${OBJFILES_READER} ${OBJFILES} ${EXEC_WRITER} ${EXEC_READER} ${EXEC}

run: ${EXEC_WRITER} ${EXEC_READER} ${EXEC}
	./${EXEC}
	