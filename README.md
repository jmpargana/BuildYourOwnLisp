# Build Your Own Lisp


This repository contains an implementation of **a** lisp dialect written in C.
The program was created following the guide from the [Build Your Own Lisp](http://www.buildyourownlisp.com/)
book by Daniel Holden and includes some extra features.

This project made me familiarize myself with C and Lisp as well as compiler 
logic.

## Table of Contents
1. [Usage](#usage)
2. [Key Differences](#keydifferences)
3. [Pure Lispy](#purelispy)
4. [Standard Library](#standardlibrary)

## Usage <a name="usage"></a>


Clone this repository, compile the source code and execute the resulting binary:

```bash
git clone https://github.com/jmpargana/BuildYourOwnLisp && cd BuildYourOwnLisp && make
```

You can either open the REPL:

```bash
./lispy
```

Or run one or more programs:

```bash
./lispy program1.lspy program2.lspy
```

Enjoy Lisp!



## Key Differences <a name="keydifferences"></a>


The macro or quote expression are represented by curly brackets
instead of the classical **'** apostrophe or quote symbol sign.

The syntax might seem odd if you're too familiar with Lisp, so I'll
provide you some examples both using the standard library as writing pure lispy.


## Pure Lispy <a name="purelispy"></a>


Some features represented here are much easier to use (the syntax is nicer) 
if you load the standard library, so make sure you read the next part as well.


- Classical mathematical operations
```lisp
lispy> + 1 (* (^ 3 4) (% 23 13))            ; comments are written after a semicolon
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

- Haskell Curry is also available 
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

- Comparison functions and conditionals
```lisp
lispy> > 10 5
1
lispy> == 5 {}
0
lispy> == {1 {2 3}} {1 {2 3}}
1
lispy> def {x y} 1 2
()
lispy> if {>= x y} {+ x y} {- x y}
-1
```


- You can the typical recursive functions
```lisp
lispy> def {len} (\ {l} {if (== l {}) {0} {+ 1 (len (tail l))}})
()
lispy> len {1 1 1 1}
4
lispy> len {1 {2 {3 2 {4}}}}
2
lispy> def {reverse} (\ {l} {if (== l {}) {{}} {join (reverse (tail l)) (head l)}})
()
lispy> reverse {1 2 3 4}
{4 3 2 1}
lispy> def {!} (\ {n} {if (<= n 0) {1} {* n (! (- n 1))}})
()
lispy> ! 5
120
lispy> ! (* 4 5)
2432902008176640000
```

* Strings and loading files

```lisp
lispy> head {"hello" "world"}
"hello"
lispy> print "Hello World!"
"Hello World!"
()
lispy> error "This is an error"
Error: This is an error
lispy> load "examples/hello.lspy" ; expression in files need to be surrounded by parentheses
"Hello World!"
()
```


## Standard Library <a name="standardlibrary"></a>

Start by loading the standard library file either in the REPL or directly on
your source file, like so:

```lisp
(load "prelude.lspy")
```

Make sure to copy the prelude file to wherever you are executing the binary from
and that the code is always surround by parentheses unlike the in the REPL.

The empty list, boolean values and logical functions are defined by name.
And the nicer function definition mentioned above as well as the Haskell Curry
are already implemented.

```lisp
(== nil {})         ; evaluates to true
(and true false)    ; evaluates to false

(fun {square x} {* x x})    ; instead of lambda

(unpack + {5 4 3})         ; works without the conversion
```


You can define a new scope and compose functions.


```lisp
(let {do (= {x} 1) (x)})
(x)                 ; Is out of scope so shows error "Unbound Symbol 'x'"

(def {mul-neg} (comp - (unpack *)))
(mul-neg {2 8})     ; Evaluates to -16
```

You can use the classical list functions provided by Haskell.


```lisp
(len {4 2 {2 3}})       ; 3
(nth 4 {1 2 3 4 5})     ; 4
(last {1 2 4})          ; 4
(take 2 {1 2 1 3})      ; {1 2}
(elem 2 {1 1 1})        ; false
(drop 2 {1 2 3 4})      ; {3 4}
(split 2 {1 2 3 4})     ; {3 4 1 2}
```


And the really powerful ones.


```lisp
; Perhaps the most needed functin in a recursive language
(map - {2 3 -7})                    ; {-2 -3 7}
(map (\ {x} {+ x 10}) {5 2})        ; {15 17}

; In the repl
lispy> map print {"hello" "world"}
"hello"
"world"
{() ()}


; filter is available as well
(filter (\ {x} {> x 5}) {6 2 1 3 4 2 3})      ; {6}


; as is fold left which is used to define sum and product
(fun {sum l} {foldl + 0 l})                 ; ()
(fun {product l} {foldl * 1 l})             ; ()

(sum {1 2 3 2 1})                           ; 9
```


Some nice syntax is also available for conditionals, such as:


```lisp
(fun {day-name x} {
    case x
        {0 "Monday"}
        {1 "Tuesday"}
        ;; You get the point
})


(fun {fib n} {
    select 
        { (== n 0) 0 }
        { (== n 1) 1 }
        { otherwise (+ (fib (- n 1)) (fib (- n 2))) }
})
```
