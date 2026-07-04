# ----------------
# Define variables
# ----------------
CURRENT_DIR := $(shell pwd)
BUILD_DIR = build
SRC_DIR = src
TARGET = Jesus
CMAKE = cmake
MAKE = make

# ---------------------------
# Default build type: Release
# ---------------------------
BUILD_TYPE ?= Release

# -----------------------------------------------------------------------
# Default target: It first builds the C++ executable using cmake and make
# -----------------------------------------------------------------------
all: $(BUILD_DIR)
	cd $(BUILD_DIR) && $(CMAKE) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) .. && $(MAKE)

# ----------------------------------------------
# Create the build directory if it doesn't exist
# ----------------------------------------------
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# --------------------------
# Run the 'jesus' executable
# --------------------------
run: $(BUILD_DIR)/jesus
	./$(BUILD_DIR)/jesus

# ---------------------------------
# Convenience targets:
#   make          → Release (fastest normal build)
#   make debug    → debugging with gdb: gdb jesus # then: run
#   make profile  → profiling with perf or valgrind:
#                     valgrind --tool=callgrind jesus benchmark.jesus
#                     kcachegrind callgrind.out.*
# ---------------------------------
release:
	$(MAKE) BUILD_TYPE=Release

debug:
	$(MAKE) BUILD_TYPE=Debug

profile:
	$(MAKE) BUILD_TYPE=RelWithDebInfo

# ------------------------
# Clean up build directory
# ------------------------
clean:
	rm -rf $(BUILD_DIR)

# Add phony targets
.PHONY: all run clean
