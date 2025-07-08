CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic -O2

SRCDIR = src
OBJDIR = obj
BINDIR = bin

SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

TARGET = $(BINDIR)/cvrp_solver
EXPERIMENT_TARGET = $(BINDIR)/experiment
TEST_TARGET = $(BINDIR)/test_solution_reader
TEST_SWAP_TARGET = $(BINDIR)/test_swap
TEST_LOCAL_SEARCH_TARGET = $(BINDIR)/test_local_search

all: $(TARGET) $(EXPERIMENT_TARGET)

$(OBJDIR):
	mkdir -p $@

$(BINDIR):
	mkdir -p $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Programa principal (sin experiment.o)
$(TARGET): $(filter-out $(OBJDIR)/experiment.o $(OBJDIR)/test_%.o,$(OBJECTS)) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Programa de experimentos (sin main.o)
$(EXPERIMENT_TARGET): $(filter-out $(OBJDIR)/main.o $(OBJDIR)/test_%.o,$(OBJECTS)) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_TARGET): $(OBJDIR)/test_solution_reader.o | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_SWAP_TARGET): $(OBJDIR)/test_swap.o | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_LOCAL_SEARCH_TARGET): $(filter-out $(OBJDIR)/main.o $(OBJDIR)/experiment.o $(OBJDIR)/test_solution_reader.o $(OBJDIR)/test_swap.o,$(OBJECTS)) $(OBJDIR)/test_local_search.o | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -rf $(OBJDIR) $(BINDIR)

.PHONY: all clean
