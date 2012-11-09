VERSION = 1.0.0
CC			= gcc
CFLAGS	= -Wall -Wextra -Werror -c
LDFLAGS	= -lm -lpthread
DBFLAGS	= -g
BIN   	= ./bin
SRC   	= ./src
TEST    = ./test

#
# +--------------------------+
# | src Files alsways needed |
# +--------------------------+
#
EVOLUTION_SRC = $(SRC)/Evolution.c
EVOLUTION_OBJ = $(BIN)/Evolution.o
SORT_SRC			= $(SRC)/sort.c
SORT_OBJ			= $(BIN)/sort.o

#
# +---------------+
# | Parallel Test |
# +---------------+
#
PARALLEL_TEST_SRC	= $(TEST)/test-parallel.c
PARALLEL_TEST_OBJ	= $(BIN)/test-parallel.o
PARALLEL_TEST_BIN	= $(BIN)/test-parallel

evolution:
	@echo "Make Evolution and depending stuff"
	$(CC) $(CFLAGS) $(EVOLUTION_SRC) -o $(EVOLUTION_OBJ)
	$(CC) $(CFLAGS) $(SORT_SRC) -o $(SORT_OBJ)

evolution-debug:
	@echo "Make Evolution and depending stuff with debugging options"
	$(CC) $(CFLAGS) $(DBFLAGS) $(EVOLUTION_SRC) -o $(EVOLUTION_OBJ)
	$(CC) $(CFLAGS) $(DBFLAGS) $(SORT_SRC) -o $(SORT_OBJ)

parallel-test: evolution
	@echo "Make parallel-test"
	$(CC) $(CFLAGS) $(PARALLEL_TEST_SRC) -o $(PARALLEL_TEST_OBJ)
	$(CC) $(LDFLAGS) $(EVOLUTION_OBJ) $(SORT_OBJ) $(PARALLEL_TEST_OBJ) -o $(PARALLEL_TEST_BIN)

parallel-test-debug: evolution-debug
	@echo "Make parallel-test with debugging options"
	$(CC) $(CFLAGS) $(DBFLAGS) $(PARALLEL_TEST_SRC) -o $(PARALLEL_TEST_OBJ)
	$(CC) $(LDFLAGS) $(EVOLUTION_OBJ) $(SORT_OBJ) $(PARALLEL_TEST_OBJ) -o $(PARALLEL_TEST_BIN)

clean:
	@echo "make clean"
	rm $(BIN)/*
