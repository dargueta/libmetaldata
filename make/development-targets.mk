.PHONY: clean-analysis valgrind massif cachegrind callgrind


MEMORY_ANALYSIS_FILE = valgrind-report.txt
HEAP_ANALYSIS_FILE = massif-report.txt
CACHE_ANALYSIS_FILE = cache-report.txt
CALL_ANALYSIS_FILE = call-report.txt
COVERAGE_FILES_GCNO = $(ALL_C_SOURCE_FILES:%.c=%.gcno)
COVERAGE_FILES_GCDA = $(ALL_C_SOURCE_FILES:%.c=%.gcda)
COVERAGE_FILES_GCOV = $(addsuffix .gcov,$(ALL_C_SOURCE_FILES))
ANALYSIS_REPORT_FILES = $(MEMORY_ANALYSIS_FILE) \
                        $(HEAP_ANALYSIS_FILE)   \
                        $(CACHE_ANALYSIS_FILE)  \
                        $(CALL_ANALYSIS_FILE)   \
                        $(COVERAGE_FILES_GCNO)  \
                        $(COVERAGE_FILES_GCDA)  \
                        $(COVERAGE_FILES_GCOV)

.PRECIOUS: $(ANALYSIS_REPORT_FILES)

analyze: clean-analysis valgrind massif cachegrind callgrind
valgrind: $(MEMORY_ANALYSIS_FILE)
massif: $(HEAP_ANALYSIS_FILE)
cachegrind: $(CACHE_ANALYSIS_FILE)
callgrind: $(CALL_ANALYSIS_FILE)


VALGRIND_BASE_COMMAND = \
    valgrind --error-exitcode=1    \
             --trace-children=yes  \
             --child-silent-after-fork=yes \
             --read-inline-info=yes


$(MEMORY_ANALYSIS_FILE): $(TEST_BINARY)
	$(VALGRIND_BASE_COMMAND)  \
	    --leak-check=full     \
	    --show-leak-kinds=all \
	    --track-origins=yes   \
	    --expensive-definedness-checks=yes \
	    --xml=yes        \
	    --xml-file=$@    \
	    --xtree-leak=yes \
	    --               \
	    $< $(ARGS)


$(HEAP_ANALYSIS_FILE): $(TEST_BINARY)
	$(VALGRIND_BASE_COMMAND) \
	    --tool=massif        \
	    --stacks=yes         \
	    --massif-out-file=$@ \
	    --                   \
	    $< $(ARGS)


$(CACHE_ANALYSIS_FILE): $(TEST_BINARY)
	$(VALGRIND_BASE_COMMAND)     \
	    --tool=cachegrind        \
	    --cachegrind-out-file=$@ \
	    --                       \
	    $< $(ARGS)


$(CALL_ANALYSIS_FILE): $(TEST_BINARY)
	$(VALGRIND_BASE_COMMAND)    \
	    --tool=callgrind        \
	    --callgrind-out-file=$@ \
	    --dump-instr=yes        \
	    --collect-jumps=yes     \
	    --                      \
	    $< $(ARGS)


clean-analysis:
	$(RM) $(ANALYSIS_REPORT_FILES) cachegrind.out.* callgrind.out.* valgrind.out.* massif.out.* xtleak.* vgcore.*
