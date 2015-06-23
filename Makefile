VERSION = 1.0.0
SRC   							= ./src
SORT_SRC 						= $(SRC)/C-Utils/Sort
THREAD_CLIENTS_SRC 	= $(SRC)/C-Utils/Thread-Clients
BIN   							= ./bin
SORT_BIN 						= $(SORT_SRC)/bin
THREAD_CLIENTS_BIN 	= $(THREAD_CLIENTS_SRC)/bin
TEST    						= ./test
CC									= gcc
CFLAGS							= -Wall -Wextra -Werror -c
LDFLAGS							= -lm -lpthread $(SORT_BIN)/sort.o \
																		$(THREAD_CLIENTS_BIN)/thread-clients.o
OTFLAGS 						= -march=native
DBFLAGS							= -g

#
# +--------------------------+
# | src Files alsways needed |
# +--------------------------+
#
EVOLUTION_SRC = $(SRC)/evolution.c
EVOLUTION_OBJ = $(BIN)/evolution.o

#
# +---------------+
# | Parallel Test |
# +---------------+
#
PARALLEL_TEST_SRC			= $(TEST)/test-parallel.c
PARALLEL_TEST_OBJ			= $(BIN)/test-parallel.o
PARALLEL_TEST_BIN			= $(BIN)/test-parallel
PARALLEL_TEST_BIN_O2	= $(BIN)/test-parallel-O2
PARALLEL_TEST_BIN_O3	= $(BIN)/test-parallel-O3

#
# +---------------------+
# | Onely Mutation Test |
# +---------------------+
#
O_MUTATE_TEST_SRC			= $(TEST)/test-only-mutate.c
O_MUTATE_TEST_OBJ			= $(BIN)/test-only-mutate.o
O_MUTATE_TEST_BIN			= $(BIN)/test-only-mutate
O_MUTATE_TEST_BIN_O2	= $(BIN)/test-only-mutate-O2
O_MUTATE_TEST_BIN_O3	= $(BIN)/test-only-mutate-O3

#
# +------------------------+
# | Use Recombination Test |
# +------------------------+
#
U_REC_TEST_SRC			= $(TEST)/test-use-recombination.c
U_REC_TEST_OBJ			= $(BIN)/test-use-recombination.o
U_REC_TEST_BIN			= $(BIN)/test-use-recombination
U_REC_TEST_BIN_O2	= $(BIN)/test-use-recombination-O2
U_REC_TEST_BIN_O3	= $(BIN)/test-use-recombination-O3

utils:
	@echo "Make depending stuff"
	mkdir -p ./bin
	make -C $(SORT_SRC)
	make -C $(THREAD_CLIENTS_SRC)

evolution: utils
	@echo "Make Evolution"
	$(CC) $(CFLAGS) $(EVOLUTION_SRC) -o $(EVOLUTION_OBJ)

evolution-O2: utils
	@echo "Make Evolution"
	$(CC) $(CFLAGS) $(OTFLAGS) -O2 $(EVOLUTION_SRC) -o $(EVOLUTION_OBJ)

evolution-O3: utils
	@echo "Make Evolution"
	$(CC) $(CFLAGS) $(OTFLAGS) -O3 $(EVOLUTION_SRC) -o $(EVOLUTION_OBJ)

evolution-debug: utils
	@echo "Make Evolution with debugging options"
	$(CC) $(CFLAGS) $(DBFLAGS) $(EVOLUTION_SRC) -o $(EVOLUTION_OBJ)

parallel-test: evolution
	@echo "Make parallel-test"
	$(CC) $(CFLAGS) $(PARALLEL_TEST_SRC) -o $(PARALLEL_TEST_OBJ)
	$(CC) $(LDFLAGS) $(EVOLUTION_OBJ) $(PARALLEL_TEST_OBJ)											\
				-o $(PARALLEL_TEST_BIN)

parallel-test-O2: clean_obj evolution-O2
	@echo "Make parallel-test O2"
	$(CC) $(CFLAGS) -D NO_OUTPUT $(PARALLEL_TEST_SRC) -o $(PARALLEL_TEST_OBJ)
	$(CC) $(LDFLAGS) $(EVOLUTION_OBJ) $(PARALLEL_TEST_OBJ) 											\
				-o $(PARALLEL_TEST_BIN_O2)

parallel-test-O3: clean_obj evolution-O3
	@echo "Make parallel-test O3"
	$(CC) $(CFLAGS) -D NO_OUTPUT $(PARALLEL_TEST_SRC) -o $(PARALLEL_TEST_OBJ)
	$(CC) $(LDFLAGS) $(EVOLUTION_OBJ) $(PARALLEL_TEST_OBJ) 											\
				-o $(PARALLEL_TEST_BIN_O3)

test-only-mutate: evolution
	@echo "Make test-only-mutate"
	$(CC) $(CFLAGS) $(O_MUTATE_TEST_SRC) -o $(O_MUTATE_TEST_OBJ)
	$(CC) $(LDFLAGS) $(EVOLUTION_OBJ) $(SORT_OBJ) $(O_MUTATE_TEST_OBJ) 					\
				-o $(O_MUTATE_TEST_BIN)

test-only-mutate-O2: clean_obj evolution-O2
	@echo "Make test-only-mutate O2"
	$(CC) $(CFLAGS) -D NO_OUTPUT $(O_MUTATE_TEST_SRC) -o $(O_MUTATE_TEST_OBJ)
	$(CC) $(LDFLAGS) $(EVOLUTION_OBJ) $(O_MUTATE_TEST_OBJ) 											\
				-o $(O_MUTATE_TEST_BIN_O2)

test-only-mutate-O3: clean_obj evolution-O3
		@echo "Make test-only-mutate O3"
	$(CC) $(CFLAGS) -D NO_OUTPUT $(O_MUTATE_TEST_SRC) -o $(O_MUTATE_TEST_OBJ)
	$(CC) $(LDFLAGS) $(EVOLUTION_OBJ) $(O_MUTATE_TEST_OBJ) 											\
				-o $(O_MUTATE_TEST_BIN_O3)

test-use-recombination: evolution
	@echo "Make test-use-recombination"
	$(CC) $(CFLAGS) $(U_REC_TEST_SRC) -o $(U_REC_TEST_OBJ)
	$(CC) $(LDFLAGS) $(EVOLUTION_OBJ) $(U_REC_TEST_OBJ) -o $(U_REC_TEST_BIN)

test-use-recombination-O2: clean_obj evolution-O2
	@echo "Make test-use-recombination O2"
	$(CC) $(CFLAGS) -D NO_OUTPUT $(U_REC_TEST_SRC) -o $(U_REC_TEST_OBJ)
	$(CC) $(LDFLAGS) $(EVOLUTION_OBJ) $(U_REC_TEST_OBJ) -o $(U_REC_TEST_BIN_O2)

test-use-recombination-O3: clean_obj evolution-O3
	@echo "Make test-use-recombination O3"
	$(CC) $(CFLAGS) -D NO_OUTPUT $(U_REC_TEST_SRC) -o $(U_REC_TEST_OBJ)
	$(CC) $(LDFLAGS) $(EVOLUTION_OBJ) $(U_REC_TEST_OBJ) -o $(U_REC_TEST_BIN_O3)


parallel-test-debug: evolution-debug
	@echo "Make parallel-test with debugging options"
	$(CC) $(CFLAGS) $(DBFLAGS) $(O_MUTATE_TEST_SRC) -o $(O_MUTATE_TEST_OBJ)
	$(CC) $(LDFLAGS) $(EVOLUTION_OBJ) $(O_MUTATE_TEST_OBJ) 											\
				-o $(O_MUTATE_TEST_BIN)

clean_obj:
	@echo "make clean obj"
	rm $(BIN)/*.o

clean:
	@echo "make clean"
	rm $(BIN)/*
