OUTPUT:=OUTPUT

CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -MMD -D${OUTPUT} #-O2
MAKEFILE_NAME = ${firstword ${MAKEFILE_LIST}}

OBJECTS1 = compiler.o extra_token_logic.o readers.o scanner.o token.o parse_table.o lr1_stack_layer.o
EXEC1 = compiler

OBJECTS = ${OBJECTS1}
DEPENDS = ${OBJECTS:.o=.d}
EXECS = ${EXEC1}

DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CXXFLAGS =-DDEBUG
else
    CXXFLAGS=-DNDEBUG
endif

####################################################3

.PHONY : all clean

all : ${EXECS}


${EXEC1} : ${OBJECTS1}
	${CXX} ${CXXFLAGS} $^ -o $@ $(CFLAGS)

${OBJECTS} : ${MAKEFILE_NAME}



-include ${DEPENDS}

clean:
	rm -f *.d ${OBJECTS} ${EXECS}
