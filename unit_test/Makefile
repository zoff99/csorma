#
# unit_test/Makefile
# ==================
# Builds unit tests directly against ../template/csorma.c (the real code).
# No fragile extraction — tests always run against current source.
#
# Usage:
#   make test_all       - build and run all tests
#   make test_str       - string utility tests
#   make test_bind      - bindvar tests
#   make test_db        - database operation tests
#   make test_schema    - schema upgrade tests
#   make test_resultset - result set tests
#   make test_thread    - threading / concurrency tests
#   make asan_all       - all tests with AddressSanitizer
#   make tsan_all       - all tests with ThreadSanitizer
#   make ubsan_all      - all tests with UBSanitizer
#   make clean          - remove generated files
#

SHELL        := /bin/bash
TEMPLATE_DIR := ../template
SQLITE_DIR   := ../sqlite
BUILD_DIR    := ./_build
BIN_DIR      := ./_bin

CC     ?= gcc
AR     ?= ar

CFLAGS := -std=c99 -Wall -Wextra -Wno-unused-function -Wno-unused-parameter \
          -Wno-unused-variable -g -O0 -pthread \
          -I.. -I$(TEMPLATE_DIR) -I$(SQLITE_DIR) -I.

SQLITE_C   := $(SQLITE_DIR)/sqlite3.c
CSORMA_C   := $(TEMPLATE_DIR)/csorma.c
CSORMA_H   := $(TEMPLATE_DIR)/csorma.h
LOGGER_C   := $(TEMPLATE_DIR)/logger.c
LOGGER_H   := $(TEMPLATE_DIR)/logger.h

.PHONY: all test_all test_str test_bind test_db test_schema test_resultset test_thread \
        asan_all tsan_all ubsan_all clean objs

all: test_all

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# ============================================================
#  Compile shared objects (done once, reused by all tests)
# ============================================================
objs: $(BUILD_DIR) $(BUILD_DIR)/sqlite3.o $(BUILD_DIR)/csorma.o $(BUILD_DIR)/logger.o

$(BUILD_DIR)/sqlite3.o: $(SQLITE_C) | $(BUILD_DIR)
	@echo "=== Compiling SQLite3 ==="
	$(CC) -c $(CFLAGS) $(CFLAGS_EXTRA) \
		-DSQLITE_THREADSAFE=1 -DSQLITE_TEMP_STORE=2 \
		-Wno-unused-parameter -Wno-implicit-fallthrough \
		-Wno-pointer-arith -Wno-sign-compare -Wno-double-promotion \
		$(SQLITE_C) -o $@

$(BUILD_DIR)/csorma.o: $(CSORMA_C) $(CSORMA_H) | $(BUILD_DIR)
	@echo "=== Compiling csorma.c ==="
	$(CC) -c $(CFLAGS) $(CFLAGS_EXTRA) $(CSORMA_C) -o $@

$(BUILD_DIR)/logger.o: $(LOGGER_C) $(LOGGER_H) | $(BUILD_DIR)
	@echo "=== Compiling logger.c ==="
	$(CC) -c $(CFLAGS) $(CFLAGS_EXTRA) $(LOGGER_C) -o $@

# ============================================================
#  Build test binaries (each links against real csorma.o)
# ============================================================
$(BIN_DIR)/test_str_utils: test_str_utils.c objs | $(BIN_DIR)
	@echo "=== Building test_str_utils ==="
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) $< \
		$(BUILD_DIR)/csorma.o $(BUILD_DIR)/logger.o $(BUILD_DIR)/sqlite3.o \
		-o $@ -lm -pthread

$(BIN_DIR)/test_bindvars: test_bindvars.c objs | $(BIN_DIR)
	@echo "=== Building test_bindvars ==="
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) $< \
		$(BUILD_DIR)/csorma.o $(BUILD_DIR)/logger.o $(BUILD_DIR)/sqlite3.o \
		-o $@ -lm -pthread

$(BIN_DIR)/test_db_ops: test_db_ops.c objs | $(BIN_DIR)
	@echo "=== Building test_db_ops ==="
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) $< \
		$(BUILD_DIR)/csorma.o $(BUILD_DIR)/logger.o $(BUILD_DIR)/sqlite3.o \
		-o $@ -lm -pthread

$(BIN_DIR)/test_schema: test_schema.c objs | $(BIN_DIR)
	@echo "=== Building test_schema ==="
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) $< \
		$(BUILD_DIR)/csorma.o $(BUILD_DIR)/logger.o $(BUILD_DIR)/sqlite3.o \
		-o $@ -lm -pthread

$(BIN_DIR)/test_resultset: test_resultset.c objs | $(BIN_DIR)
	@echo "=== Building test_resultset ==="
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) $< \
		$(BUILD_DIR)/csorma.o $(BUILD_DIR)/logger.o $(BUILD_DIR)/sqlite3.o \
		-o $@ -lm -pthread

$(BIN_DIR)/test_threading: test_threading.c objs | $(BIN_DIR)
	@echo "=== Building test_threading ==="
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) $< \
		$(BUILD_DIR)/csorma.o $(BUILD_DIR)/logger.o $(BUILD_DIR)/sqlite3.o \
		-o $@ -lm -pthread

# ============================================================
#  Run targets
# ============================================================
test_str: $(BIN_DIR)/test_str_utils
	@echo ""
	@echo "============================================================"
	@echo "  Running: String Utility Tests"
	@echo "============================================================"
	./$(BIN_DIR)/test_str_utils

test_bind: $(BIN_DIR)/test_bindvars
	@echo ""
	@echo "============================================================"
	@echo "  Running: Bind Variable Tests"
	@echo "============================================================"
	./$(BIN_DIR)/test_bindvars

test_db: $(BIN_DIR)/test_db_ops
	@echo ""
	@echo "============================================================"
	@echo "  Running: Database Operation Tests"
	@echo "============================================================"
	./$(BIN_DIR)/test_db_ops

test_schema: $(BIN_DIR)/test_schema
	@echo ""
	@echo "============================================================"
	@echo "  Running: Schema Upgrade Tests"
	@echo "============================================================"
	./$(BIN_DIR)/test_schema

test_resultset: $(BIN_DIR)/test_resultset
	@echo ""
	@echo "============================================================"
	@echo "  Running: Result Set Tests"
	@echo "============================================================"
	./$(BIN_DIR)/test_resultset

test_thread: $(BIN_DIR)/test_threading
	@echo ""
	@echo "============================================================"
	@echo "  Running: Threading / Concurrency Tests"
	@echo "============================================================"
	./$(BIN_DIR)/test_threading

test_all:
	@echo ""
	@echo "============================================================"
	@echo "  RUNNING ALL TEST SUITES"
	@echo "============================================================"
	@FAILURES=0; \
	$(MAKE) test_str       || FAILURES=$$((FAILURES+1)); \
	$(MAKE) test_bind      || FAILURES=$$((FAILURES+1)); \
	$(MAKE) test_db        || FAILURES=$$((FAILURES+1)); \
	$(MAKE) test_schema    || FAILURES=$$((FAILURES+1)); \
	$(MAKE) test_resultset || FAILURES=$$((FAILURES+1)); \
	$(MAKE) test_thread    || FAILURES=$$((FAILURES+1)); \
	echo ""; \
	echo "============================================================"; \
	if [ $$FAILURES -eq 0 ]; then \
		echo "  ALL 6 TEST SUITES PASSED"; \
	else \
		echo "  $$FAILURES TEST SUITE(S) FAILED"; \
	fi; \
	echo "============================================================"; \
	exit $$FAILURES

# ============================================================
#  Sanitized builds
# ============================================================
asan_all: clean
	@echo "=== ASAN BUILD ==="
	$(MAKE) CFLAGS_EXTRA="-fsanitize=address -fno-omit-frame-pointer" test_all

tsan_all: clean
	@echo "=== TSAN BUILD ==="
	$(MAKE) CFLAGS_EXTRA="-fsanitize=thread -fno-omit-frame-pointer" test_all

ubsan_all: clean
	@echo "=== UBSAN BUILD ==="
	$(MAKE) CFLAGS_EXTRA="-fsanitize=undefined,float-divide-by-zero -fno-sanitize-recover=all" test_all

# ============================================================
#  Clean
# ============================================================
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	rm -f *.db *.db-journal *.db-wal *.db-shm
	@echo "Cleaned."
