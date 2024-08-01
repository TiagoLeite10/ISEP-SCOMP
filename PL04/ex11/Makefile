# Makefile

# Variáveis
SOURCES_LIBS_SHM = ../libs/my_shared_memory_lib.c
OBJFILES_LIBS_SHM = ../libs/my_shared_memory_lib.o

SOURCES_LIBS_SEM = ../libs/my_semaphore_lib.c
OBJFILES_LIBS_SEM = ../libs/my_semaphore_lib.o

INCLUDES_SELLER = ../libs/my_shared_memory_lib.h ../libs/my_semaphore_lib.h
SOURCES_SELLER = ../libs/my_shared_memory_lib.c ../libs/my_semaphore_lib.c seller.c
OBJFILES_SELLER = ../libs/my_shared_memory_lib.o ../libs/my_semaphore_lib.o seller.o
EXEC_SELLER = seller

INCLUDES_CUSTOMER = ../libs/my_shared_memory_lib.h ../libs/my_semaphore_lib.h generate_random_value.h
SOURCES_CUSTOMER = ../libs/my_shared_memory_lib.c ../libs/my_semaphore_lib.c generate_random_value.c customer.c
OBJFILES_CUSTOMER = ../libs/my_shared_memory_lib.o ../libs/my_semaphore_lib.o generate_random_value.o customer.o
EXEC_CUSTOMER = customer

INCLUDES = ../libs/my_shared_memory_lib.h ../libs/my_semaphore_lib.h
SOURCES = ../libs/my_shared_memory_lib.c ../libs/my_semaphore_lib.c main.c
OBJFILES = ../libs/my_shared_memory_lib.o ../libs/my_semaphore_lib.o main.o
EXEC = ex11

# Regras para o sufixo
.SUFFIXES : .c .o

.c.o:
	gcc -Wall -g -c $<

${EXEC_SELLER}: ${OBJFILES_SELLER}
	gcc -Wall -g -o ${EXEC_SELLER} ${OBJFILES_SELLER} -lrt -lpthread

${OBJFILES_SELLER}: ${SOURCES_SELLER} ${INCLUDES_SELLER}

${EXEC_CUSTOMER}: ${OBJFILES_CUSTOMER}
	gcc -Wall -g -o ${EXEC_CUSTOMER} ${OBJFILES_CUSTOMER} -lrt -lpthread

${OBJFILES_CUSTOMER}: ${SOURCES_CUSTOMER} ${INCLUDES_CUSTOMER}

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
	rm -f ${OBJFILES_SELLER} ${OBJFILES_CUSTOMER} ${OBJFILES} ${EXEC_CUSTOMER} ${EXEC_SELLER} ${EXEC}

run: ${EXEC_SELLER} ${EXEC_CUSTOMER} ${EXEC}
	./${EXEC}
	