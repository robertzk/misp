# Multi-list processor language

This repository forms the home of **MISP**, the multi-list processor language. This language
is an experimental language envisionable as a superset of LISP — and also a chance a to learn some C++11. 

MISP is distinct from [LISP](https://en.wikipedia.org/wiki/Lisp_(programming_language)) through
its emphasis that the parenthetical grouping expressions are [*typed*](https://en.wikipedia.org/wiki/Type_theory).
Thus, whereas in LISP one can say `(foo bar baz)` is an application of the `foo` function to the
operands `bar` and `baz`, in MISP the tokens `(` and `)` themselves have semantics.

Specifically, one can say `[foo bar baz]`, or `{foo bar baz}`, or even `[: foo bar baz :]`, which
define new spontaneous types for the associated groupings. These parenthetical
"operators" such as `[,]`, `{,}` and `[:,:]` are termed **enclosers**.

MISP recognizes enclosers through the special primitive **atomic encloser tokens**,
namely characters in `([{<` and their respective closers `)]}>`. 

Under the hood, MISP transforms a parsed file or expression into an [AST](https://en.wikipedia.org/wiki/Abstract_syntax_tree),
with the parent representing the encloser operand (e.g., `[:,:]`).

Thus, an expression such as `(foo bar baz)` is actually parsed as `( -> foo -> bar -> baz`,
where `(` is seen as the parent of a tree and `foo`, `bar` and `baz` are its branches.

## Example MISP programs


```
# hello.misp
# A program is by default surrounded with the `{` encloser.
print "hello world"
```

```
# operators.misp
x = (a + b + {c * d} + {: z \ y :} )
```

Programs are side-effect free and cannot have external effects on the world unless
the programmer specifies an **ontological binding** or ontobinding. Such a binding correlates
the processing of the AST with external side-effects. For example, the
default `base` ontobinding ties `print` to output in [`stdout`](https://en.wikipedia.org/wiki/Standard_streams).

Thus, running `misp hello.misp` will print "hello world", but one
can also remove all ontobindings through `misp --onto= hello.misp`. Such
a program would have no side effects, as all of its effects on the external
world have been removed, and thus it would print nothing.

## Running MISP

First, run `make` to create the `bin/misp` executable.

### Parsing

You can see how MISP parses your program by passing in the `--parse` keyword
followed by a MISP program file.

```
bin/misp --parse <(echo 'foo [ baz bux ] {: coo  caw ( jim jum ) :}')

MISP input:
-------------------------
foo [ baz bux ] {: coo  caw ( jim jum ) :}
-------------------------

MISP code parsed:
-------------------------
(foo [baz bux] {: coo caw (jim jum) :})
-------------------------
```

Another example:

```
bin/misp --parse <(echo 'a (b c d [f e] {{ a t }} [: x :])')

MISP input:
-------------------------
a (b c d [f e] {{ a t }} [: x :]
-------------------------

MISP code parsed:
-------------------------
(a (b c d [f e] {{ a t }} [: x :]))
-------------------------
```

One more example:

```
bin/misp --parse examples/nested-groups.misp

MISP input:
-------------------------
this is {
  my cool program (
    and it goes like this [a b c]
  )
}
-------------------------

MISP code parsed:
-------------------------
(this is {my cool program (and it goes like this [a b c])})
-------------------------
```

```
bin/misp --parse examples/nested-hello-world.misp
MISP input:
-------------------------
execute
  (print hello)
  (print world)
-------------------------

MISP code parsed:
-------------------------
(execute (print hello) (print world))
-------------------------
```

### Executing

You can execute MISP programs by simply passing the filename to the misp executable.
Buy default, this will use the (rather trivial) **print ontobinding**, which just
traverses the AST of a MISP program and prints `X` to standard out whenever
it sees `(print X)`.

```
bin/misp <(echo 'print helloworld')
helloworld
```

```
bin/misp examples/nested-hello-world.misp
helloworld
```

```
bin/misp <(echo '(print this) is a (print misp) program')
thismisp
```

## License

MISP is licensed under the [MIT License](LICENSE) and freely available for modification,
distribution or commercial use.

