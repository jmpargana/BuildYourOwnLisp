/**********************************************************************
 *
 * Contains all the definitions for the constructors of the 
 * lval struct (tree)
 * it contains a pointer to multiple lvals which contain
 * a number of children, the contents, the type which
 * can be error, symbol, num or s-expression
 *
 * Author: Joao Pargana
 *
 *
 **********************************************************************/


#include "../include/repl.h"


/* Construct a pointer to a new Number lval */
lval* lval_num(long x) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_NUM;
    v->num = x;
    return v;
}


/* Construct a pointer to a new Error lval */
lval* lval_err(char* fmt, ...) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_ERR;

    /* Create a va list and initialize it */
    va_list va;
    va_start(va, fmt);

    /* Allocate 512 bytes of space */
    v->err = malloc(512);

    /* printf the error string with a maximum of 511 characters */
    vsnprintf(v->err, 511, fmt, va);

    /* Reallocate to number of bytes actually used */
    v->err = realloc(v->err, strlen(v->err)+1);

    /* Cleanup out va list */
    va_end(va);

    return v;
}


/* Contruct a pointer to a new Symbol lval */
lval* lval_sym(char* s) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_SYM;

    v->sym = malloc(strlen(s) + 1);
    strcpy(v->sym, s);

    return v;
}


/* A pointer to a new empty Sexpr lval */
lval* lval_sexpr(void) {
    lval* v = malloc(sizeof(lval));
    
    v->type = LVAL_SEXPR;
    v->count = 0;
    v->cell = NULL;

    return v;
}


/* A pointer to a new empty Qexpr lval */
lval* lval_qexpr(void) {
    lval* v = malloc(sizeof(lval));

    v->type = LVAL_QEXPR;
    v->count = 0;
    v->cell = NULL;

    return v;
}


lval* lval_fun(lbuiltin func) {
    lval* v = malloc(sizeof(lval));

    v->type = LVAL_FUN;
    v->builtin = func;

    return v;
}


lval* lval_copy(lval* v) {

    lval* x = malloc(sizeof(lval));
    x->type = v->type;

    switch (v->type) {

        /* Copy Functions and Numbers Directly */
        case LVAL_NUM: x->num = v->num; break;
        case LVAL_FUN: 

            if (v->builtin) {
                x->builtin = v->builtin;                

            } else {
                x->builtin = NULL;
                x->env = lenv_copy(v->env);
                x->formals = lval_copy(v->formals);
                x->body = lval_copy(v->body);
            }
            break;

        /* Copy strings using malloc and strcpy */
        case LVAL_ERR:
            x->err = malloc(strlen(v->err) + 1);
            strcpy(x->err, v->err); break;

        case LVAL_SYM:
            x->sym = malloc(strlen(v->sym) + 1);
            strcpy(x->sym, v->sym); break;

        case LVAL_STR:
            x->str = malloc(strlen(v->str) + 1);
            strcpy(x->str, v->str); break;

        /* Copy lists by copying each sub-expression */
        case LVAL_SEXPR:
        case LVAL_QEXPR:
            x->count = v->count;
            x->cell = malloc(sizeof(lval*) * x->count);

            for (int i = 0; i < x->count; i++) {
                x->cell[i] = lval_copy(v->cell[i]);
            }
            break;
    }

    return x;
}


/* Delete a lval pointer */
void lval_del(lval* v) {
    
    switch (v->type) {
        case LVAL_NUM: break;           // Do nothing for number
        case LVAL_FUN: 

            if (!v->builtin) {
                lenv_del(v->env);
                lval_del(v->formals);
                lval_del(v->body);
            }
            break;

        /* Free the strings */
        case LVAL_ERR: free(v->err); break;
        case LVAL_SYM: free(v->sym); break;
        case LVAL_STR: free(v->str); break;

        case LVAL_QEXPR:
        case LVAL_SEXPR:
            /* Free all elements inside recursively */
            for (int i = 0; i < v->count; i++) {
                lval_del(v->cell[i]);
            }

            /* And itself */
            free(v->cell);
            break;

    }

    /* Finally free the memory allocated for the "lval" struct itself */
    free(v);
}


lval* lval_lambda(lval* formals, lval* body) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_FUN;

    /* Set Builtin to NULL */
    v->builtin = NULL;

    /* Build new environment */
    v->env = lenv_new();

    /* Set Formals and Body */
    v->formals = formals;
    v->body = body;

    return v;
}


lval* lval_str(char* s) {
    lval* v = malloc(sizeof(lval));

    v->type = LVAL_STR;
    v->str = malloc(strlen(s) + 1);
    strcpy(v->str, s);

    return v;
}
