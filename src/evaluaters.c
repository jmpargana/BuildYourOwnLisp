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
        LASSERT_TYPE(op, a, i, LVAL_NUM);
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
    LASSERT_NUM("head", a, 1);
    LASSERT_TYPE("head", a, 0, LVAL_QEXPR);
    LASSERT_NOT_EMPTY("head", a, 0);

    /* Otherwise take first argument */
    lval* v = lval_take(a, 0);

    while (v->count > 1) { lval_del(lval_pop(v, 1)); }

    return v;
}


lval* builtin_tail(lenv* e, lval* a) {
    LASSERT_NUM("tail", a, 1);
    LASSERT_TYPE("tail", a, 0, LVAL_QEXPR);
    LASSERT_NOT_EMPTY("tail", a, 0);

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
    LASSERT_NUM("eval", a, 1);
    LASSERT_TYPE("eval", a, 0, LVAL_QEXPR);

    lval* x = lval_take(a, 0);
    x->type = LVAL_SEXPR;
    
    return lval_eval(e, x);
}


lval* builtin_join(lenv* e, lval* a) {

    for (int i = 0; i < a->count; i++) {
        LASSERT_TYPE("join", a, i, LVAL_QEXPR);
    }

    lval* x = lval_pop(a, 0);

    while (a->count) {
        lval* y = lval_pop(a, 0);
        x = lval_join(x, y);
    }

    lval_del(a);
    return x;
}

lval* lval_join(lval* x, lval* y) {

    /* For each cell in 'y' add it to 'x' */
    for (int i = 0; i < y->count; i++) {
        x = lval_add(x, y->cell[i]);
    }

    free(y->cell);
    free(y);

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

        lval* err = lval_err(
            "S-Expression starts with incorrect type."
            "Got %s, Expected %s.",
            ltype_name(f->type), ltype_name(LVAL_FUN));

        lval_del(f); lval_del(v);
        return err;
    }

    /* Call builtin with operator */
    lval* result = lval_call(e, f, v);
    lval_del(f);
    return result;
}



lval* builtin_lambda(lenv* e, lval* a) {
    /* Check two arguments, each of which are Q-Expressions */
    LASSERT_NUM("\\", a, 2);
    LASSERT_TYPE("\\", a, 0, LVAL_QEXPR);
    LASSERT_TYPE("\\", a, 1, LVAL_QEXPR);

    /* Check first Q-Expression contains only Symbols */
    for (int i = 0; i < a->cell[0]->count; i++) {
        LASSERT(a, (a->cell[0]->cell[i]->type == LVAL_SYM),
                "Cannot define non-symbol. "
                "Got %s, Expected %s.",
                ltype_name(a->cell[0]->cell[i]->type), ltype_name(LVAL_SYM));
    }

    /* Pop first two arguments and pass them to lval_lambda */
    lval* formals = lval_pop(a, 0);
    lval* body = lval_pop(a, 0);
    lval_del(a);

    return lval_lambda(formals, body);
}


lval* lval_call(lenv* e, lval* f, lval* a) {

    /* If Builtin then simply call that */
    if (f->builtin) { return f->builtin(e, a); }

    /* Record Argument Counts */
    int given = a->count;
    int total = f->formals->count;

    /* While arguments still remain to be processed */
    while (a->count) {

        /* If we've ran out of formal arguments to bind */
        if (f->formals->count == 0) {
            lval_del(a); return lval_err(
                "Function passed too many arguments. "
                "Got %i, Expected &i.", given, total);
        }

        /* Pop the first symbol from the formals */
        lval* sym = lval_pop(f->formals, 0);

        /* Pop the next argument from the list */
        lval* val = lval_pop(a, 0);

        /* Bind a copy into the function's environment */
        lenv_put(f->env, sym, val);

        /* Delete symbol and value */
        lval_del(sym); lval_del(val);
    }

    /* Argument list is now bound so can be cleaned up */
    lval_del(a);

    /* If all formals have been bound evaluate */
    if (f->formals->count == 0) {

        /* Set environment parent to evaluation environment */
        f->env->par = e;

        /* Evaluate and return */
        return builtin_eval(
            f->env, lval_add(lval_sexpr(), lval_copy(f->body)));

    } else {
        /* Otherwise return partially evaluated function */
        return lval_copy(f);
    }
}
