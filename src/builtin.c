/**********************************************************************
 *
 * This file contians the definitions of the builtin operator functions
 *
 * Author: Joao Pargana
 *
 **********************************************************************/


#include "../include/repl.h"


lval* builtin_def(lenv* e, lval* a) {
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
            "Function 'def' passed incorrect type!");

    /* First argument is symbol list */
    lval* syms = a->cell[0];

    /* Ensure all elements of first list are symbols */
    for (int i = 0; i < syms->count; i++) {
        LASSERT(a, syms->cell[i]->type == LVAL_SYM,
                "Function 'def' cannot define non-symbol");
    }

    /* Check correct number of symbols and values */
    LASSERT(a, syms->count == a->count-1,
            "Function 'def' connot define incorrect "
            "number of values to symbols");

    /* Assign copies of values to symbols */
    for (int i = 0; i < syms->count; i++) {
        lenv_put(e, syms->cell[i], a->cell[i+1]);
    }

    lval_del(a);
    return lval_sexpr();
}


lval* builtin_add(lenv* e, lval* a) {
    return builtin_op(e, a, "+");
}


lval* builtin_sub(lenv* e, lval* a) {
    return builtin_op(e, a, "-");
}


lval* builtin_mul(lenv* e, lval* a) {
    return builtin_op(e, a, "*");
}


lval* builtin_div(lenv* e, lval* a) {
    return builtin_op(e, a, "/");
}


lval* builtin_mod(lenv* e, lval* a) {
    return builtin_op(e, a, "%");
}


lval* builtin_pow(lenv* e, lval* a) {
    return builtin_op(e, a, "^");
}
