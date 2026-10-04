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

.PHONY: all help bench clean exercises solutions compare \
        test1 test2 test3 test4 test5 test6 \
        compare1 compare2 compare3 compare4 compare5 compare6 \
        demo1 demo2 demo3 demo4 demo5 demo6

all: help

help:
	@echo "================================================================================"
	@echo "  Cache Locality & Data-Oriented Design (DoD) - Makefile Commands"
	@echo "================================================================================"
	@echo "  Individual Exercise Testing:"
	@echo "    make test1       - Test Exercise 01 (Spatial Locality)"
	@echo "    make test2       - Test Exercise 02 (Pointer Chasing)"
	@echo "    make test3       - Test Exercise 03 (Struct Packing)"
	@echo "    make test4       - Test Exercise 04 (AoS vs SoA)"
	@echo "    make test5       - Test Exercise 05 (Hot/Cold Splitting)"
	@echo "    make test6       - Test Exercise 06 (Flattened AST)"
	@echo "    make exercises   - Test all exercises"
	@echo ""
	@echo "  Compare Your Solution vs Reference Solution (3-Way Benchmark):"
	@echo "    make compare1    - Compare Exercise 01 vs Reference"
	@echo "    make compare2    - Compare Exercise 02 vs Reference"
	@echo "    make compare3    - Compare Exercise 03 vs Reference"
	@echo "    make compare4    - Compare Exercise 04 vs Reference"
	@echo "    make compare5    - Compare Exercise 05 vs Reference"
	@echo "    make compare6    - Compare Exercise 06 vs Reference"
	@echo "    make compare     - Run 3-way comparisons for all exercises"
	@echo ""
	@echo "  Reference & Demonstrations:"
	@echo "    make bench       - Run all standalone concept benchmarks"
	@echo "    make demo1..6    - Run demo for specific topic"
	@echo "    make solutions   - Verify all reference solutions"
	@echo "    make clean       - Remove all compiled test binaries"
	@echo "================================================================================"

# Demos
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

demo1: 01_spatial_locality/row_vs_col 01_spatial_locality/stride_benchmark
	@./01_spatial_locality/row_vs_col
	@./01_spatial_locality/stride_benchmark

demo2: 02_pointer_chasing/list_vs_array
	@./02_pointer_chasing/list_vs_array

demo3: 03_struct_packing/struct_packing
	@./03_struct_packing/struct_packing

demo4: 04_aos_vs_soa/aos_vs_soa
	@./04_aos_vs_soa/aos_vs_soa

demo5: 05_hot_cold_splitting/hot_cold
	@./05_hot_cold_splitting/hot_cold

demo6: 06_indices_vs_pointers/ast_tree
	@./06_indices_vs_pointers/ast_tree

# -----------------------------------------------------------------------------
# Individual Exercise Tests (Single problem testing)
# -----------------------------------------------------------------------------
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

# -----------------------------------------------------------------------------
# 3-Way Comparisons: Baseline vs Your Solution vs Reference Solution
# -----------------------------------------------------------------------------
compare1:
	@$(CC) $(CFLAGS) -Dblur_optimized=ref_blur_optimized -c solutions/01_solution.c -o solutions/01_sol.o
	@$(CC) $(CFLAGS) 01_spatial_locality/exercise.c 01_spatial_locality/compare.c solutions/01_sol.o -o 01_spatial_locality/compare_ex01
	@-./01_spatial_locality/compare_ex01
	@rm -f solutions/01_sol.o

compare2:
	@$(CC) $(CFLAGS) -Dqueue_init=ref_queue_init -Dqueue_free=ref_queue_free -Dqueue_push=ref_queue_push -Dqueue_pop=ref_queue_pop -c solutions/02_solution.c -o solutions/02_sol.o
	@$(CC) $(CFLAGS) 02_pointer_chasing/exercise.c 02_pointer_chasing/compare.c solutions/02_sol.o -o 02_pointer_chasing/compare_ex02
	@-./02_pointer_chasing/compare_ex02
	@rm -f solutions/02_sol.o

compare3:
	@$(CC) $(CFLAGS) -c solutions/03_solution.c -o solutions/03_sol.o
	@$(CC) $(CFLAGS) 03_struct_packing/exercise.c 03_struct_packing/compare.c solutions/03_sol.o -o 03_struct_packing/compare_ex03
	@-./03_struct_packing/compare_ex03
	@rm -f solutions/03_sol.o

compare4:
	@$(CC) $(CFLAGS) -Dparticle_soa_init=ref_particle_soa_init -Dparticle_soa_free=ref_particle_soa_free -Dparticle_soa_update=ref_particle_soa_update -c solutions/04_solution.c -o solutions/04_sol.o
	@$(CC) $(CFLAGS) 04_aos_vs_soa/exercise.c 04_aos_vs_soa/compare.c solutions/04_sol.o -o 04_aos_vs_soa/compare_ex04
	@-./04_aos_vs_soa/compare_ex04
	@rm -f solutions/04_sol.o

compare5:
	@$(CC) $(CFLAGS) -Dtick_hot_units=ref_tick_hot_units -c solutions/05_solution.c -o solutions/05_sol.o
	@$(CC) $(CFLAGS) 05_hot_cold_splitting/exercise.c 05_hot_cold_splitting/compare.c solutions/05_sol.o -o 05_hot_cold_splitting/compare_ex05
	@-./05_hot_cold_splitting/compare_ex05
	@rm -f solutions/05_sol.o

compare6:
	@$(CC) $(CFLAGS) -Dflatten_tree=ref_flatten_tree -Deval_compact_ast=ref_eval_compact_ast -c solutions/06_solution.c -o solutions/06_sol.o
	@$(CC) $(CFLAGS) 06_indices_vs_pointers/exercise.c 06_indices_vs_pointers/compare.c solutions/06_sol.o -o 06_indices_vs_pointers/compare_ex06
	@-./06_indices_vs_pointers/compare_ex06
	@rm -f solutions/06_sol.o

compare: compare1 compare2 compare3 compare4 compare5 compare6

# -----------------------------------------------------------------------------
# Reference Solutions Verification
# -----------------------------------------------------------------------------
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
	rm -f 01_spatial_locality/compare_ex01 02_pointer_chasing/compare_ex02 03_struct_packing/compare_ex03
	rm -f 04_aos_vs_soa/compare_ex04 05_hot_cold_splitting/compare_ex05 06_indices_vs_pointers/compare_ex06
	rm -f solutions/test_sol* solutions/*.o
