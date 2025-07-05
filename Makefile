# ----------------
# Define variables
# ----------------
CURRENT_DIR := $(shell pwd)
BUILD_DIR = build
SRC_DIR = src
TARGET = Jesus
CMAKE = cmake
MAKE = make

# -----------------------------------------------------------------------
# Default target: It first builds the C++ executable using cmake and make
# -----------------------------------------------------------------------
all: $(BUILD_DIR)
	cd $(BUILD_DIR) && $(CMAKE) .. && $(MAKE)

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

# ------------------------
# Clean up build directory
# ------------------------
clean:
	rm -rf $(BUILD_DIR)

# Add phony targets
.PHONY: all run clean
