/**********************************************************************
 *
 * This file contians the definitions of the builtin operator functions
 *
 * Author: Joao Pargana
 *
 **********************************************************************/


#include "../include/repl.h"


lval* builtin_def(lenv* e, lval* a) {

    LASSERT_TYPE("def", a, 0, LVAL_QEXPR);

    /* First argument is symbol list */
    lval* syms = a->cell[0];

    /* Ensure all elements of first list are symbols */
    for (int i = 0; i < syms->count; i++) {
        LASSERT(a, syms->cell[i]->type == LVAL_SYM,
            "Function 'def' cannot define non-symbol. "
            "Got %s, Expected %s.",
            ltype_name(syms->cell[i]->type), ltype_name(LVAL_SYM));
    }

    /* Check correct number of symbols and values */
    LASSERT(a, syms->count == a->count-1,
            "Function 'def' passed too many arguments for symbols. "
            "Got %i, Expected %i.",
            syms->count, a->count-1);

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
