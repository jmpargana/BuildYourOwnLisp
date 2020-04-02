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
lval* lval_err(char* m) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_ERR;

    v->err = malloc(strlen(m) + 1);
    strcpy(v->err, m);

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
    v->fun = func;

    return v;
}


lval* lval_copy(lval* v) {

    lval* x = malloc(sizeof(lval));
    x->type = v->type;

    switch (v->type) {

        /* Copy Functions and Numbers Directly */
        case LVAL_NUM: x->num = v->num; break;
        case LVAL_FUN: x->fun = v->fun; break;

        /* Copy strings using malloc and strcpy */
        case LVAL_ERR:
            x->err = malloc(strlen(v->err) + 1);
            strcpy(x->err, v->err); break;

        case LVAL_SYM:
            x->sym = malloc(strlen(v->sym) + 1);
            strcpy(x->sym, v->sym); break;

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
        case LVAL_FUN: break;           // or function pointers (variables)

        /* Free the strings */
        case LVAL_ERR: free(v->err); break;
        case LVAL_SYM: free(v->sym); break;

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


lenv* lenv_new(void) {
    lenv* e = malloc(sizeof(lenv));

    e->count = 0;
    e->syms = NULL;
    e->vals = NULL;

    return e;
}


void lenv_del(lenv* e) {

    for (int i = 0; i < e->count; i++) {
        free(e->syms[i]);
        lval_del(e->vals[i]);
    }

    free(e->syms);
    free(e->vals);
    free(e);
}


lval* lenv_get(lenv* e, lval* k) {

    /* Iterate over all items in environment */
    for (int i = 0; i < e->count; i++) {
        /* Check if the stored string matches the symbol string */
        /* If it does, return a copy of the value */
        if (strcmp(e->syms[i], k->sym) == 0) {
            return lval_copy(e->vals[i]);
        }
    }
    /* If no symbol found return error */
    return lval_err("Unbound symbol!");
}


void lenv_put(lenv* e, lval* k, lval* v) {

    /* Iterate over allitems in environment */
    /* This is to see if variable already exists */
    for (int i = 0; i < e->count; i++) {

        /* If variable is found delete item at that position */
        if (strcmp(e->syms[i], k->sym) == 0) {
            lval_del(e->vals[i]);
            e->vals[i] = lval_copy(v);
            return;
        }
    }

    /* If no existing entry found allocate space for new entry */
    e->count++;
    e->vals = realloc(e->vals, sizeof(lval*) * e->count);
    e->syms = realloc(e->syms, sizeof(char*) * e->count);

    /* Copy contents of lval and symbol string into new location */
    e->vals[e->count-1] = lval_copy(v);
    e->syms[e->count-1] = malloc(strlen(k->sym)+1);
    strcpy(e->syms[e->count-1], k->sym);
}
