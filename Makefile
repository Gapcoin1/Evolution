VERSION = 1.0.0
BIN   							= ./bin
CC									= gcc
CFLAGS							= -Wall -Wextra -Werror -c -Winline 
LDFLAGS							= -lm -pthread
OTFLAGS 						= -march=native
DBFLAGS							= -g -D DEBUG
LIB									= src/C-Utils

LIB_SRC = $(shell find $(LIB) -type f -name '*.c'|grep ".*/test/.*" -v)
LIB_OBJ = $(LIB_SRC:%.c=%.o)

prepare:
	@mkdir -p $(BIN)

%.o: %.c
	$(CC) $(CFLAGS) $^ -o $@

compile: $(LIB_OBJ)

link: prepare compile
LDFLAGS += $(LIB_OBJ)

debug: CC += $(DBFLAGS)
debug: clean link evolution evolution-O2 evolution-O3 all


evolution: link
	@echo "Make Evolution"
	$(CC) $(CFLAGS) src/evolution.c -o src/evolution.o

evolution-O2: link
	@echo "Make Evolution"
	$(CC) $(CFLAGS) $(OTFLAGS) -O2 src/evolution.c -o src/evolution-O2.o

evolution-O3: link
	@echo "Make Evolution"
	$(CC) $(CFLAGS) $(OTFLAGS) -O3 src/evolution.c -o src/evolution-O3.o

evolution-debug: link
	@echo "Make Evolution with debugging options"
	$(CC) $(CFLAGS) $(DBFLAGS) src/evolution.c -o src/evolution-debug.o

last-test: CFLAGS += -D NO_OUTPUT
last-test: evolution
	@echo "Make last-test"
	$(CC) $(CFLAGS) test/last_test.c -o test/last_test.o
	$(CC) $(LDFLAGS) src/evolution.o test/last_test.o											\
				-o $(BIN)/last_test

last-test-O2: CFLAGS += -D NO_OUTPUT
last-test-O2: evolution-O2
	@echo "Make last-test O2"
	$(CC) $(CFLAGS) -D NO_OUTPUT test/last_test.c -o test/last_test.o
	$(CC) $(LDFLAGS) src/evolution-O2.o test/last_test.o 											\
				-o $(BIN)/last_test-O2

last-test-O3: CFLAGS += -D NO_OUTPUT
last-test-O3: evolution-O3
	@echo "Make last-test O3"
	$(CC) $(CFLAGS) -D NO_OUTPUT test/last_test.c -o test/last_test.o
	$(CC) $(LDFLAGS) src/evolution-O3.o test/last_test.o 											\
				-o $(BIN)/last_test-O3

parallel-test: CFLAGS += -D NO_OUTPUT
parallel-test: evolution
	@echo "Make parallel-test"
	$(CC) $(CFLAGS) test/test-parallel.c -o test/test-parallel.o
	$(CC) $(LDFLAGS) src/evolution.o test/test-parallel.o											\
				-o $(BIN)/test-parallel
				
parallel-test-O2: CFLAGS += -D NO_OUTPUT
parallel-test-O2: evolution-O2
	@echo "Make parallel-test O2"
	$(CC) $(CFLAGS) -D NO_OUTPUT test/test-parallel.c -o test/test-parallel.o
	$(CC) $(LDFLAGS) src/evolution-O2.o test/test-parallel.o 											\
				-o $(BIN)/test-parallel-O2

parallel-test-O3: CFLAGS += -D NO_OUTPUT
parallel-test-O3: evolution-O3
	@echo "Make parallel-test O3"
	$(CC) $(CFLAGS) -D NO_OUTPUT test/test-parallel.c -o test/test-parallel.o
	$(CC) $(LDFLAGS) src/evolution-O3.o test/test-parallel.o 											\
				-o $(BIN)/test-parallel-O3

tsp-test: CFLAGS += -D NO_OUTPUT
tsp-test: evolution
	@echo "Make tsp-test"
	$(CC) $(CFLAGS) test/tsp.c -o test/tsp.o
	$(CC) $(LDFLAGS) src/evolution.o test/tsp.o											\
				-o $(BIN)/tsp

tsp-test-O2: CFLAGS += -D NO_OUTPUT
tsp-test-O2: evolution-O2
	@echo "Make tsp-test O2"
	$(CC) $(CFLAGS) -D NO_OUTPUT $(OTFLAGS) test/tsp.c -o test/tsp.o
	$(CC) $(LDFLAGS) src/evolution-O2.o test/tsp.o 											\
				-o $(BIN)/tsp-O2

tsp-test-O3: CFLAGS += -D NO_OUTPUT
tsp-test-O3: evolution-O3
	@echo "Make tsp-test O3"
	$(CC) $(CFLAGS) -D NO_OUTPUT $(OTFLAGS) test/tsp.c -o test/tsp.o
	$(CC) $(LDFLAGS) src/evolution-O3.o test/tsp.o 											\
				-o $(BIN)/tsp-O3
				
test-only-mutate: evolution
	@echo "Make test-only-mutate"
	$(CC) $(CFLAGS) test/test-only-mutate.c -o test/test-only-mutate.o
	$(CC) $(LDFLAGS) src/evolution.o test/test-only-mutate.o 			\
				-o $(BIN)/test-only-mutate

test-only-mutate-O2: evolution-O2
	@echo "Make test-only-mutate O2"
	$(CC) $(CFLAGS) -D NO_OUTPUT test/test-only-mutate.c 											\
									-o test/test-only-mutate.o
	$(CC) $(LDFLAGS) src/evolution-O2.o test/test-only-mutate.o 									\
				-o $(BIN)/test-only-mutate-O2

test-only-mutate-O3: evolution-O3
		@echo "Make test-only-mutate O3"
	$(CC) $(CFLAGS) -D NO_OUTPUT test/test-only-mutate.c 										\
									-o test/test-only-mutate.o
	$(CC) $(LDFLAGS) src/evolution-O3.o test/test-only-mutate.o 								\
				-o $(BIN)/test-only-mutate-O3

test-use-recombination: evolution
	@echo "Make test-use-recombination"
	$(CC) $(CFLAGS) test/test-use-recombination.c 											\
									-o test/test-use-recombination.o
	$(CC) $(LDFLAGS) src/evolution.o test/test-use-recombination.o 			\
									-o $(BIN)/test-use-recombination

test-use-recombination-O2: evolution-O2
	@echo "Make test-use-recombination O2"
	$(CC) $(CFLAGS) -D NO_OUTPUT test/test-use-recombination.c 								\
									-o test/test-use-recombination.o
	$(CC) $(LDFLAGS) src/evolution-O2.o test/test-use-recombination.o 						\
									-o $(BIN)/test-use-recombination-O2

test-use-recombination-O3: evolution-O3
	@echo "Make test-use-recombination O3"
	$(CC) $(CFLAGS) -D NO_OUTPUT test/test-use-recombination.c 						\
									-o test/test-use-recombination.o
	$(CC) $(LDFLAGS) src/evolution-O3.o test/test-use-recombination.o 				\
									-o $(BIN)/test-use-recombination-O3


parallel-test-debug: evolution-debug
	@echo "Make parallel-test with debugging options"
	$(CC) $(CFLAGS) $(DBFLAGS) test/test-only-mutate.c 						\
									-o test/test-only-mutate.o
	$(CC) $(LDFLAGS) src/evolution-debug.o test/test-only-mutate.o 			\
				-o $(BIN)/test-only-mutate

clean_obj:
	@echo "make clean obj"
	@find .|grep "\.o$$"| xargs -I{} rm -rf "{}"

clean: clean_obj
	@echo "make clean"
	rm -rf $(BIN)

all: parallel-test-debug test-use-recombination-O3 test-use-recombination-O2 \
		 test-use-recombination test-only-mutate-O3 test-only-mutate-O2 				 \
		 test-only-mutate tsp-test-O3 tsp-test-O2 tsp-test parallel-test-O3 		 \
		 parallel-test-O2 parallel-test last-test-O3 last-test-O2 last-test
