#ifndef REPL_H
#define REPL_H


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <editline/readline.h>
#include "./mpc.h"


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
 * Define Struct
 *
 * 
 ************************************************************************************/


/* Forward declarations */
struct lval;
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;


/* Can be Error, Number, Symbol or S-Expression */
enum { LVAL_ERR, LVAL_NUM, LVAL_SYM, 
       LVAL_FUN, LVAL_SEXPR, LVAL_QEXPR };


typedef lval*(*lbuiltin)(lenv*, lval*);


/* lval struct has a type enum, error enum or value */
typedef struct lval {
    int type;
    long num;

    /* Error and Symbol types have some string data */
    char* err;
    char* sym;

    lbuiltin fun;

    /* Count and Pointer to a list of "lval* " */
    int count;
    struct lval** cell;

} lval;


struct lenv {
    int count;
    char** syms;
    lval** vals;
};



/************************************************************************************/


/**
 * Definitions in constructors.c 
 *
 */
lval* lval_num(long);
lval* lval_err(char*, ...);
lval* lval_sym(char*);
lval* lval_sexpr(void);
lval* lval_qexpr(void);
lval* lval_fun(lbuiltin);

lval* lval_copy(lval*);
void lval_del(lval*);


lenv* lenv_new(void);
void lenv_del(lenv*);
lval* lenv_get(lenv*, lval*);
void lenv_put(lenv*, lval*, lval*);



/**
 * Readers 
 *
 */
lval* lval_read_num(mpc_ast_t* t);
lval* lval_read(mpc_ast_t* t);



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




/** 
 * Printers
 *
 */
void lval_print(lval* v);
void lval_expr_print(lval* v, char open, char close);
void lval_println(lval* v);
char* ltype_name(int);





#endif
