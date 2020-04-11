OUTPUT:=OUTPUT

CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -MMD -D${OUTPUT} #-O2
MAKEFILE_NAME = ${firstword ${MAKEFILE_LIST}}

OBJECTS1 = build_environment.o compiler.o extra_token_logic.o readers.o scanner.o token.o parse_table.o lr1_stack_layer.o weeder.o environment.o helper_functions.o package.o type_linker.o name_checker.o type_checker.o

EXEC1 = joosc

OBJECTS = ${OBJECTS1}
DEPENDS = ${OBJECTS:.o=.d}
EXECS = ${EXEC1}

DEBUG ?= 0
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
