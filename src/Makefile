# Makefile for CVRP Project
# Author: CVRP Team
# Date: 2024

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic
OPTFLAGS = -O2 -DNDEBUG
DEBUGFLAGS = -g -O0 -DDEBUG

# Directories
SRCDIR = src
OBJDIR = obj
BINDIR = bin
INSTANCESDIR = instancias/2l-cvrp-0
SOLUTIONSDIR = instancias/2l-cvrp-0/soluciones

# Target executable
TARGET = $(BINDIR)/cvrp_solver
TEST_TARGET = $(BINDIR)/test_solution_reader

# Source files
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Header files
HEADERS = $(wildcard $(SRCDIR)/*.h)

# Main source files (excluding test files)
MAIN_SOURCES = $(filter-out $(SRCDIR)/test_%.cpp, $(SOURCES))
MAIN_OBJECTS = $(MAIN_SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Test source files
TEST_SOURCES = $(filter $(SRCDIR)/test_%.cpp, $(SOURCES))
TEST_OBJECTS = $(TEST_SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Default target
all: $(TARGET)

# Create directories
$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# Main executable
$(TARGET): $(MAIN_OBJECTS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -o $@ $^

# Test executable
$(TEST_TARGET): $(TEST_OBJECTS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) -o $@ $^

# Compile object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS) | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -c -o $@ $<

# Debug version
debug: CXXFLAGS += $(DEBUGFLAGS)
debug: $(TARGET)

# Release version (default)
release: CXXFLAGS += $(OPTFLAGS)
release: $(TARGET)

# Clean build artifacts
clean:
	rm -rf $(OBJDIR) $(BINDIR)
	rm -f *.o *.exe

# Clean everything including LaTeX artifacts
distclean: clean
	rm -f *.aux *.log *.toc *.pdf

# Install (copy to system path)
install: $(TARGET)
	sudo cp $(TARGET) /usr/local/bin/

# Uninstall
uninstall:
	sudo rm -f /usr/local/bin/cvrp_solver

# Run with a sample instance
run: $(TARGET)
	@echo "Running CVRP solver with sample instance..."
	@if [ -f "$(INSTANCESDIR)/E045-04f.dat" ]; then \
		$(TARGET) $(INSTANCESDIR)/E045-04f.dat; \
	else \
		echo "Sample instance not found. Please provide a path to a VRP instance file."; \
		echo "Usage: make run INSTANCE=path/to/instance.dat"; \
	fi

# Run with specific instance
run-instance: $(TARGET)
	@if [ -z "$(INSTANCE)" ]; then \
		echo "Please specify an instance file:"; \
		echo "make run-instance INSTANCE=path/to/instance.dat"; \
		exit 1; \
	fi
	@echo "Running CVRP solver with instance: $(INSTANCE)"
	$(TARGET) $(INSTANCE)

# Test the solution reader
test: $(TEST_TARGET)
	@echo "Running solution reader test..."
	@if [ -f "$(SOLUTIONSDIR)/E045-04F.HRE" ]; then \
		$(TEST_TARGET) $(SOLUTIONSDIR)/E045-04F.HRE; \
	else \
		echo "Test solution file not found."; \
	fi

# List available instances
list-instances:
	@echo "Available instances:"
	@ls -1 $(INSTANCESDIR)/*.dat 2>/dev/null || echo "No instances found in $(INSTANCESDIR)"

# List available solutions
list-solutions:
	@echo "Available solutions:"
	@ls -1 $(SOLUTIONSDIR)/*.HRE 2>/dev/null || echo "No solutions found in $(SOLUTIONSDIR)"

# Check dependencies
check-deps:
	@echo "Checking dependencies..."
	@which $(CXX) >/dev/null || (echo "Error: $(CXX) not found. Please install a C++ compiler." && exit 1)
	@echo "✓ C++ compiler found: $(CXX)"
	@echo "✓ Compiler version:"
	@$(CXX) --version | head -1

# Show help
help:
	@echo "CVRP Project Makefile"
	@echo "====================="
	@echo ""
	@echo "Available targets:"
	@echo "  all              - Build the main executable (default)"
	@echo "  debug            - Build with debug flags"
	@echo "  release          - Build with optimization flags"
	@echo "  clean            - Remove build artifacts"
	@echo "  distclean        - Remove all generated files"
	@echo "  install          - Install to system path"
	@echo "  uninstall        - Remove from system path"
	@echo "  run              - Run with sample instance"
	@echo "  run-instance     - Run with specific instance (INSTANCE=path)"
	@echo "  test             - Run solution reader test"
	@echo "  list-instances   - Show available instances"
	@echo "  list-solutions   - Show available solutions"
	@echo "  check-deps       - Check system dependencies"
	@echo "  help             - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make                                    # Build the project"
	@echo "  make run                                # Run with sample instance"
	@echo "  make run-instance INSTANCE=path/to/file.dat"
	@echo "  make debug                              # Build debug version"
	@echo "  make clean                              # Clean build files"

# Phony targets
.PHONY: all debug release clean distclean install uninstall run run-instance test list-instances list-solutions check-deps help

# Dependencies
$(OBJDIR)/main.o: $(SRCDIR)/main.cpp $(SRCDIR)/VRPLIBReader.h $(SRCDIR)/clarke_wright.h $(SRCDIR)/SolutionReader.h $(SRCDIR)/solution.h
$(OBJDIR)/VRPLIBReader.o: $(SRCDIR)/VRPLIBReader.cpp $(SRCDIR)/VRPLIBReader.h
$(OBJDIR)/clarke_wright.o: $(SRCDIR)/clarke_wright.cpp $(SRCDIR)/clarke_wright.h $(SRCDIR)/VRPLIBReader.h $(SRCDIR)/solution.h
$(OBJDIR)/nearest_neighbor.o: $(SRCDIR)/nearest_neighbor.cpp $(SRCDIR)/nearest_neighbor.h $(SRCDIR)/VRPLIBReader.h $(SRCDIR)/solution.h
$(OBJDIR)/solution.o: $(SRCDIR)/solution.cpp $(SRCDIR)/solution.h
$(OBJDIR)/SolutionReader.o: $(SRCDIR)/SolutionReader.cpp $(SRCDIR)/SolutionReader.h $(SRCDIR)/solution.h
$(OBJDIR)/test_solution_reader.o: $(SRCDIR)/test_solution_reader.cpp $(SRCDIR)/SolutionReader.h $(SRCDIR)/solution.h 