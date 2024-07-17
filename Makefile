.PHONY: all clean

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic
FLEX = flex
BISON = bison

all: hw3

hw3: lex.yy.c parser.tab.cpp
	$(CXX) $(CXXFLAGS) -o hw3 lex.yy.c parser.tab.cpp *.cpp

lex.yy.c: scanner.lex
	$(FLEX) scanner.lex

parser.tab.cpp parser.tab.hpp: parser.ypp
	$(BISON) -d parser.ypp

clean:
	rm -f lex.yy.c parser.tab.cpp parser.tab.hpp hw3