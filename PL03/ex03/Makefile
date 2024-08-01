# Makefile

# Variáveis
INCLUDES_WRITER = generate_random_value.h
SOURCES_WRITER = generate_random_value.c writer.c
OBJFILES_WRITER = generate_random_value.o writer.o
EXEC_WRITER = writer

SOURCES_READER = reader.c
OBJFILES_READER = reader.o
EXEC_READER = reader

# Regras para o sufixo
.SUFFIXES : .c .o

.c.o:
	gcc -Wall -g -c $<

${EXEC_WRITER}: ${OBJFILES_WRITER}
	gcc -Wall -g -o ${EXEC_WRITER} ${OBJFILES_WRITER} -lrt

${OBJFILES_WRITER}: ${SOURCES_WRITER}

${EXEC_READER}: ${OBJFILES_READER}
	gcc -Wall -g -o ${EXEC_READER} ${OBJFILES_READER} -lrt

${OBJFILES_READER}: ${SOURCES_READER}

clean:
	rm -f ${OBJFILES_WRITER} ${OBJFILES_READER} ${EXEC_WRITER} ${EXEC_READER}

run: ${EXEC_WRITER} ${EXEC_READER}
	./${EXEC_WRITER} & ./${EXEC_READER}
