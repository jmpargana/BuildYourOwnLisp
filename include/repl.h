#ifndef REPL_H
#define REPL_H


/**
 * So its portable in windows
 */
#ifdef _WIN32


static char buffer[2048];

char* readline(char* prompt) {
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);
    char* cpy = malloc(strlen(buffer) + 1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy)-1] = '\0';
    return cpy;
}

void add_history(char* unused) {}

#else 
#include <readline/readline.h>
#include <readline/history.h>
#endif


/**
 * This headers should be the same for both
 * I have no idea if they'll work on windows
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpc.h"


/**
 * Define Error Macros
 *
 */
#define LASSERT(args, cond, fmt, ...)                                   \
    if (!(cond)) {                                                      \
        lval* err = lval_err(fmt, ##__VA_ARGS__);                       \
        lval_del(args);                                                 \
        return err;                                                     \
    }

#define LASSERT_TYPE(func, args, index, expect)                         \
    LASSERT(args, args->cell[index]->type == expect,                    \
            "Function '%s' passed incorrect type for argument %i."      \
            "Got %s, Expected %s.",                                     \
            func, index, ltype_name(args->cell[index]->type), ltype_name(expect))

#define LASSERT_NUM(func, args, num)                                    \
    LASSERT(args, args->count == num,                                   \
            "Function '%s' passed incorrect number of arguments."       \
            "Got %i, Expected %i.",                                     \
            func, args->count, num)


#define LASSERT_NOT_EMPTY(func, args, index)                            \
    LASSERT(args, args->cell[index]->count != 0,                        \
            "Function '%s' passed {} for argument %i.",                 \
            func, index)





/************************************************************************************
 *
 * Define Lisp Value and Lisp Environment Structs
 *
 * 
 ************************************************************************************/


/* Forward declarations */
struct lval;
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;


/* Can be Error, Number, Symbol or S-Expression */
enum { LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_STR,
       LVAL_FUN, LVAL_SEXPR, LVAL_QEXPR };


typedef lval*(*lbuiltin)(lenv*, lval*);


/* lval struct has a type enum, error enum or value */
typedef struct lval {
    int type;

    /* Basic */
    long num;
    char* err;
    char* sym;
    char* str;

    /* Function */
    lbuiltin builtin;
    lenv* env;
    lval* formals;
    lval* body;

    /* Expression */
    int count;
    struct lval** cell;

} lval;


struct lenv {
    lenv* par;
    int count;
    char** syms;
    lval** vals;
};



/************************************************************************************/


/**
 * Declaration of global variables
 */
mpc_parser_t* Number;
mpc_parser_t* Symbol;
mpc_parser_t* String;
mpc_parser_t* Comment;
mpc_parser_t* Sexpr;
mpc_parser_t* Qexpr;
mpc_parser_t* Expr;
mpc_parser_t* Lispy;



/************************************************************************************/


/**
 * Definitions of constructors
 *
 */
lval* lval_num(long);
lval* lval_err(char*, ...);
lval* lval_sym(char*);
lval* lval_sexpr(void);
lval* lval_qexpr(void);
lval* lval_str(char*);

lval* lval_fun(lbuiltin);
lval* lval_lambda(lval*, lval*);
lval* lval_call(lenv*, lval*, lval*);

lval* lval_copy(lval*);
void lval_del(lval*);


lenv* lenv_new(void);
void lenv_del(lenv*);
lval* lenv_get(lenv*, lval*);
void lenv_put(lenv*, lval*, lval*);
lenv* lenv_copy(lenv*);
void lenv_def(lenv*, lval*, lval*);



/**
 * Readers 
 *
 */
lval* lval_read_num(mpc_ast_t* t);
lval* lval_read(mpc_ast_t* t);
lval* lval_read_str(mpc_ast_t*);



/**
 * Evaluaters 
 *
 */
lval* lval_add(lval* v, lval* x);
lval* lval_eval_sexpr(lenv*, lval*);
lval* lval_eval(lenv*, lval* v);
lval* lval_take(lval* v, int i);
void lenv_add_builtin(lenv*, char*, lbuiltin);
void lenv_add_builtins(lenv*);


/**
 * Builtins
 *
 */
lval* builtin(lval*, char*);
lval* builtin_op(lenv*, lval*, char*);
lval* builtin_def(lenv*, lval*);
lval* builtin_put(lenv*, lval*);
lval* builtin_var(lenv*, lval*, char*);
lval* builtin_lambda(lenv*, lval*);

/* String manipulation */
lval* builtin_load(lenv*, lval*);
lval* builtin_error(lenv*, lval*);
lval* builtin_print(lenv*, lval*);

/* List Functions */
lval* builtin_head(lenv*, lval* a);
lval* builtin_tail(lenv*, lval* a);
lval* builtin_list(lenv*, lval* a);
lval* builtin_eval(lenv*, lval* a);
lval* builtin_join(lenv*, lval* a);
lval* lval_join(lval* x, lval* y);

/* Operators */
lval* builtin_add(lenv*, lval*);
lval* builtin_sub(lenv*, lval*);
lval* builtin_mul(lenv*, lval*);
lval* builtin_div(lenv*, lval*);
lval* builtin_mod(lenv*, lval*);
lval* builtin_pow(lenv*, lval*);

/* Comparisons */
lval* builtin_ord(lenv*, lval*, char*);
int  lval_eq(lval*, lval*);
lval* builtin_cmp(lenv*, lval*, char*);
lval* builtin_if(lenv*, lval*);

lval* builtin_eq(lenv*, lval*);
lval* builtin_ne(lenv*, lval*);
lval* builtin_gt(lenv*, lval*);
lval* builtin_lt(lenv*, lval*);
lval* builtin_ge(lenv*, lval*);
lval* builtin_le(lenv*, lval*);




/** 
 * Printers
 *
 */
void lval_print(lval* v);
void lval_expr_print(lval* v, char open, char close);
void lval_println(lval* v);
void lval_print_str(lval*);
char* ltype_name(int);





#endif
