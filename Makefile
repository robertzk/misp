# osiris binary is not always up-to-date despite what make thinks so label as phony
.PHONY: test misp deps

all: misp

misp:
	clang++ -Wall -std=c++11 misp.cpp -o misp

deps:
	deps

tests:
	test
