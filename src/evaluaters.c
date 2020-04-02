/**********************************************************************
 *
 * This file contians the definitions of the lval evaluaters
 * Given an lval tree with all the data read in the REPL
 * perform the evaluation recursively to get the results and clean
 * the memory.
 *
 * Author: Joao Pargana
 *
 *
 **********************************************************************/


#include "../include/repl.h"


lval* lval_add(lval* v, lval* x) {
    v->count++;

    v->cell = realloc(v->cell, sizeof(lval*) * v->count);
    v->cell[v->count-1] = x;

    return v;
}


lval* lval_eval(lenv* e, lval* v) {

    if (v->type == LVAL_SYM) {

        lval* x = lenv_get(e, v);
        lval_del(v);
        return x;
    }

    if (v->type == LVAL_SEXPR) { return lval_eval_sexpr(e, v); }
    return v;
}


lval* lval_pop(lval* v, int i) {
    /* Find the item at "i" */
    lval* x = v->cell[i];

    /* Shift the memory following the item at "i" over the top of it */
    memmove(&v->cell[i], &v->cell[i+1], sizeof(lval*) * (v->count-i-1));

    /* Decrease the count of items in the list */
    v->count--;

    /* Reallocate the memory used */
    v->cell = realloc(v->cell, sizeof(lval*) * v->count);
    return x;
}


lval* lval_take(lval* v, int i) {
    lval* x = lval_pop(v, i);
    lval_del(v);
    return x;
}



lval* builtin_op(lenv* e, lval* a, char* op) {

    /* Ensure all arguments are numbers */
    for (int i = 0; i < a->count; i++) {
        if (a->cell[i]->type != LVAL_NUM) {
            lval_del(a);
            return lval_err("Cannot operator on non number!");
        }
    }

    /* Pop the first element */
    lval* x = lval_pop(a, 0);

    /* If no arguments and sub then perform unary negation */
    if ((strcmp(op, "-") == 0) && a->count == 0) { x->num = -x->num; }

    /* While there are still elements remaining */
    while (a->count > 0) {

        /* Pop the next element */
        lval* y = lval_pop(a, 0);

        /* Perform operation */
        if (strcmp(op, "+") == 0) { x->num += y->num; }
        if (strcmp(op, "-") == 0) { x->num -= y->num; }

        if (strcmp(op, "*") == 0) { x->num *= y->num; }
        if (strcmp(op, "%") == 0) { x->num %= y->num; }

        if (strcmp(op, "^") == 0) { x->num = pow(x->num, y->num); }
        if (strcmp(op, "/") == 0) {
            if (y->num == 0) {
                lval_del(x); 
                x = lval_err("Division By Zero!"); 
                break;
            } else {
                x->num /= y->num;
            }
        }

        /* Delete element now finished with */
        lval_del(y);
    }

    /* Delete input expression and return result */
    lval_del(a);
    return x;
}


lval* builtin_head(lenv* e, lval* a) {
    LASSERT(a, a->count == 1,
            "Function 'tail' passed too many arguments!");

    LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
            "Function 'head' passed incorrect type!");

    LASSERT(a, a->cell[0]->count != 0,
            "Function 'head' passed {}!")

    /* Otherwise take first argument */
    lval* v = lval_take(a, 0);

    while (v->count > 1) { lval_del(lval_pop(v, 1)); }

    return v;
}


lval* builtin_tail(lenv* e, lval* a) {
    LASSERT(a, a->count == 1,
            "Function 'tail' passed too many arguments!");
    
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
            "Function 'tail' passed incorrect type!");

    LASSERT(a, a->cell[0]->count != 0,
            "Function 'tail' passed {}!")

    /* Take first argument */
    lval* v = lval_take(a, 0);

    /* Delete first element and return */
    lval_del(lval_pop(v, 0));

    return v;
}


lval* builtin_list(lenv* e, lval* a) {
    a->type = LVAL_QEXPR;
    return a;
}


lval* builtin_eval(lenv* e, lval* a) {
    LASSERT(a, a->count == 1,
            "Function 'eval' passed too many arguments!");

    LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
            "Function 'eval' passed incorrect type!");


    lval* x = lval_take(a, 0);
    x->type = LVAL_SEXPR;
    
    return lval_eval(e, x);
}


lval* builtin_join(lenv* e, lval* a) {

    for (int i = 0; i < a->count; i++) {
        LASSERT(a, a->cell[i]->type == LVAL_QEXPR,
                "Function 'join' passed incorrect type");
    }

    lval* x = lval_pop(a, 0);

    while (a->count) {
        x = lval_join(x, lval_pop(a, 0));
    }

    lval_del(a);
    return x;
}

lval* lval_join(lval* x, lval* y) {

    /* For each cell in 'y' add it to 'x' */
    while (y->count) {
        x = lval_add(x, lval_pop(y, 0));
    }

    /* Delete the empty 'y' and return 'x' */
    lval_del(y);
    return x;
}




lval* lval_eval_sexpr(lenv* e, lval* v) {

    /* Evaluate Children */
    for (int i = 0; i < v->count; i++) {
        v->cell[i] = lval_eval(e, v->cell[i]);
    }

    /* Error Checking */
    for (int i = 0; i < v->count; i++) {
        if (v->cell[i]->type == LVAL_ERR) { return lval_take(v, i); }
    }

    /* Empty Expression */
    if (v->count == 0) { return v; }

    /* Single Expression */
    if (v->count == 1) { return lval_take(v, 0); }

    /* Ensure first element is Symbol */
    lval* f = lval_pop(v, 0);
    if (f->type != LVAL_FUN) {
        lval_del(f); lval_del(v);
        return lval_err("First element is not a function!");
    }

    /* Call builtin with operator */
    lval* result = f->fun(e, v);
    lval_del(f);
    return result;
}


