# Build Your Own Lisp


This repository contains an implementation of **a** lisp dialect written in C.
The program was created following the guide from the [Build Your Own Lisp](http://www.buildyourownlisp.com/)
book by Daniel Holden and includes some extra features.

This project made me familiarize myself with C and Lisp as well as compiler 
logic.

## Usage

Clone this repository, compile the source code and execute the resulting binary:

```bash
git clone https://github.com/jmpargana/BuildYourOwnLisp && cd BuildYourOwnLisp

make

./repl
```

Enjoy Lisp!



## Key Differences

The macro or quote expression are represented by curly brackets
instead of the classical **'** apostrophe or quote symbol sign.

The syntax might seem odd if you're too familiar with Lisp, so I'll
provide you some examples.

- Classical mathematical operations
```lisp
lispy> + 1 (* (^ 3 4) (% 23 13))
811
```

- Builtin functions
```lisp
lispy> head {hello world}
{hello}
lispy> (eval (head {+ - -})) 10 2
12
```


- Define global variables
```lisp
lispy> def {x} 100
()
lispy> x
100
lispy> def {a b} 5 6
()
lispy> def {arglist} {a b x}
()
lispy> def arglist 1 2 3
()
lispy> list a b x
{1 2 3}
```

- Use lambda functions or save them in variable
```lisp
lispy> (\ {x} {* x x}) 5
25
lispy> def {add-mul} (\ {x y} {+ x (* x y)})
()
lispy> add-mul 10 20
210
lispy> def {add-mul-ten} (add-mul 10)
()
lispy> add-mul-ten 50
510
```

- You can even provide a much nicer syntax to the lambda
```lisp
lispy> def {fun} (\ {args body} {def (head args) (\ (tail args) body)})
()
lispy> fun {add-together x y} {+ x y}
()
lispy> add-together 10 30
40
```

- Haskell Curry is also available (variable argument number)
```lisp
lispy> fun {unpack f xs} {eval (join (list f) xs)}
()
lispy> fun {pack f & xs} {f xs}
()
lispy> def {uncurry} pack
()
lispy> def {curry} unpack
()
lispy> curry + {5 6 7}
18
lispy> uncurry head 5 6 7
{5}
```


