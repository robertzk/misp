# osiris binary is not always up-to-date despite what make thinks so label as phony
.PHONY: misp tests deps test-ast test-tokenizer

all: misp

misp:
	clang++ -Wall -std=c++11 ast.cpp tokenizer.cpp misp.cpp -o bin/misp

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
