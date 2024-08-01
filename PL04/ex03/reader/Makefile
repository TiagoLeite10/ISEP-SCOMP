# Makefile

# Variáveis
SOURCES_LIBS_SEM = ../../libs/my_shared_memory_lib.c
OBJFILES_LIBS_SEM = ../../libs/my_shared_memory_lib.o

INCLUDES = ../../libs/my_shared_memory_lib.h
SOURCES = ../../libs/my_shared_memory_lib.c reader.c
OBJFILES = ../../libs/my_shared_memory_lib.o reader.o
EXEC = ex02_reader

# Regras para o sufixo
.SUFFIXES : .s .c .o

.s.o .c.o:
	gcc -Wall -g -c $<

${EXEC}: ${OBJFILES}
	gcc -Wall -g -o ${EXEC} ${OBJFILES} -lrt

# Regra explícita para gerar my_shared_memory_lib.o
${OBJFILES_LIBS_SEM}: ${SOURCES_LIBS_SEM}
	gcc -Wall -g -c $< -o $@

${OBJFILES}: ${SOURCES} ${INCLUDES}

clean:
	rm -f ${OBJFILES} ${EXEC}

run: ${EXEC}
	./${EXEC}
