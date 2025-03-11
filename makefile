CPP_FILE = shell.cpp
TARGET = shell

# OS specific
ifeq ($(OS), Windows_NT)
	TARGET := $(TARGET).exe
	RM = del
	RUN_PREFIX =
else
	RM = rm -f
	RUN_PREFIX = ./
endif

# Usage: make
$(TARGET): $(CPP_FILE)
	@echo "Building project"
	g++ $(CPP_FILE) -o $(TARGET)

# Usage: make run
run: $(TARGET)
	@echo "running the project"
	$(RUN_PREFIX)$(TARGET)

# Usage: make clean
clean: $(TARGET)
	@echo "Cleaning artifacts"
	$(RM) $(TARGET)

# These commands should run everytime.
.PHONY: run clean
