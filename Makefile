CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -pthread

.PHONY: all run clean

all: exportador

exportador: main.cpp crow_all.h
	$(CXX) $(CXXFLAGS) main.cpp -o exportador

run: exportador
	./exportador

clean:
	rm -f exportador exportador_test exportador_test.exe
