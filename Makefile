# osiris binary is not always up-to-date despite what make thinks so label as phony
.PHONY: misp tests deps test-ast test-tokenizer test-execution

all: misp

misp:
	clang++ -Wall -std=c++11 ast.cpp tokenizer.cpp execution.cpp misp.cpp -o bin/misp

deps:
	deps

tests:
	test

test-ast:
	clang++ -Wall -std=c++11 ast.cpp test-ast.cpp -o bin/test-ast
	bin/test-ast

test-tokenizer:
	clang++ -Wall -std=c++11 ast.cpp tokenizer.cpp test-tokenizer.cpp -o bin/test-tokenizer
	bin/test-tokenizer

test-execution:
	clang++ -Wall -std=c++11 ast.cpp tokenizer.cpp execution.cpp test-execution.cpp -o bin/test-execution
	bin/test-execution
