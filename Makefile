# osiris binary is not always up-to-date despite what make thinks so label as phony
.PHONY: test misp deps

all: misp

misp:
	clang++ -Wall -std=c++11 misp.cpp ast.cpp -o bin/misp

deps:
	deps

tests:
	test

test-ast:
	clang++ -Wall -std=c++11 ast.cpp test-ast.cpp -o bin/test-ast
	bin/test-ast
