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


/* Delete a lval pointer */
void lval_del(lval* v) {
    
    switch (v->type) {
        case LVAL_NUM: break;           // Do nothing for number

        /* Free the strings */
        case LVAL_ERR: free(v->err); break;
        case LVAL_SYM: free(v->sym); break;

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

