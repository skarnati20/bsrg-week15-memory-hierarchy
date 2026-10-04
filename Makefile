CC ?= clang
CFLAGS ?= -O3 -Wall -Wextra -std=c99

DEMO_SRCS = \
	01_spatial_locality/row_vs_col.c \
	01_spatial_locality/stride_benchmark.c \
	02_pointer_chasing/list_vs_array.c \
	03_struct_packing/struct_packing.c \
	04_aos_vs_soa/aos_vs_soa.c \
	05_hot_cold_splitting/hot_cold.c \
	06_indices_vs_pointers/ast_tree.c

DEMO_BINS = $(DEMO_SRCS:.c=)

.PHONY: all run bench exercises solutions clean help \
        test1 test2 test3 test4 test5 test6

all: bench

help:
	@echo "Available make targets:"
	@echo "  make bench       - Run all core memory locality benchmarks & demonstrations"
	@echo "  make exercises   - Compile and test all your exercise solutions"
	@echo "  make test1..6    - Test a specific exercise (e.g. make test1)"
	@echo "  make solutions   - Run test harness against reference solutions"
	@echo "  make clean       - Remove all compiled binaries"

# Build demo benchmarks
%: %.c common/benchmark.h
	$(CC) $(CFLAGS) $< -o $@

bench: $(DEMO_BINS)
	@echo "\n>>> RUNNING ALL CACHE LOCALITY & DOD DEMONSTRATIONS <<<\n"
	@./01_spatial_locality/row_vs_col
	@./01_spatial_locality/stride_benchmark
	@./02_pointer_chasing/list_vs_array
	@./03_struct_packing/struct_packing
	@./04_aos_vs_soa/aos_vs_soa
	@./05_hot_cold_splitting/hot_cold
	@./06_indices_vs_pointers/ast_tree

# Exercise targets
test1:
	@$(CC) $(CFLAGS) 01_spatial_locality/exercise.c 01_spatial_locality/runner.c -o 01_spatial_locality/test_ex01
	@-./01_spatial_locality/test_ex01

test2:
	@$(CC) $(CFLAGS) 02_pointer_chasing/exercise.c 02_pointer_chasing/runner.c -o 02_pointer_chasing/test_ex02
	@-./02_pointer_chasing/test_ex02

test3:
	@$(CC) $(CFLAGS) 03_struct_packing/exercise.c 03_struct_packing/runner.c -o 03_struct_packing/test_ex03
	@-./03_struct_packing/test_ex03

test4:
	@$(CC) $(CFLAGS) 04_aos_vs_soa/exercise.c 04_aos_vs_soa/runner.c -o 04_aos_vs_soa/test_ex04
	@-./04_aos_vs_soa/test_ex04

test5:
	@$(CC) $(CFLAGS) 05_hot_cold_splitting/exercise.c 05_hot_cold_splitting/runner.c -o 05_hot_cold_splitting/test_ex05
	@-./05_hot_cold_splitting/test_ex05

test6:
	@$(CC) $(CFLAGS) 06_indices_vs_pointers/exercise.c 06_indices_vs_pointers/runner.c -o 06_indices_vs_pointers/test_ex06
	@-./06_indices_vs_pointers/test_ex06

exercises: test1 test2 test3 test4 test5 test6

# Solution verification targets
solutions:
	@echo "\n>>> VERIFYING ALL REFERENCE SOLUTIONS <<<\n"
	@$(CC) $(CFLAGS) solutions/01_solution.c 01_spatial_locality/runner.c -o solutions/test_sol01 && ./solutions/test_sol01
	@$(CC) $(CFLAGS) solutions/02_solution.c 02_pointer_chasing/runner.c -o solutions/test_sol02 && ./solutions/test_sol02
	@$(CC) $(CFLAGS) -DUSE_SOLUTION_HEADER solutions/03_solution.c 03_struct_packing/runner.c -o solutions/test_sol03 && ./solutions/test_sol03
	@$(CC) $(CFLAGS) solutions/04_solution.c 04_aos_vs_soa/runner.c -o solutions/test_sol04 && ./solutions/test_sol04
	@$(CC) $(CFLAGS) solutions/05_solution.c 05_hot_cold_splitting/runner.c -o solutions/test_sol05 && ./solutions/test_sol05
	@$(CC) $(CFLAGS) solutions/06_solution.c 06_indices_vs_pointers/runner.c -o solutions/test_sol06 && ./solutions/test_sol06

clean:
	rm -f $(DEMO_BINS)
	rm -f 01_spatial_locality/test_ex01 02_pointer_chasing/test_ex02 03_struct_packing/test_ex03
	rm -f 04_aos_vs_soa/test_ex04 05_hot_cold_splitting/test_ex05 06_indices_vs_pointers/test_ex06
	rm -f solutions/test_sol*
