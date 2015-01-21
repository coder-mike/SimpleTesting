
CFLAGS = -I. -Wall -std=c++11
LDFLAGS =
CC = g++

TEST_EXE = tests.exe

SOURCES = \
	simple_testing.cpp \
	debug_new_and_delete.cpp

OBJECTS = $(SOURCES:.cpp=.o)

.PHONY: runtests

.cpp.o:
	$(CC) -c -o $@ $(CFLAGS) $<

runtests: $(TEST_EXE)
	./$(TEST_EXE)

$(TEST_EXE): ${OBJECTS}
	$(CC) $(LDFLAGS) $^ -o $@

