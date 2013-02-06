
CFLAGS = -I. -Wall -std=c++11

CPPFILES = \
	simple_testing.cpp \
	debug_new_and_delete.cpp

all: ${CPPFILES}
	g++ -c $^ $(CFLAGS)
