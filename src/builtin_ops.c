/**********************************************************************
 *
 * This file contians the definitions of the builtin operator functions
 *
 * Author: Joao Pargana
 *
 **********************************************************************/


#include "../include/repl.h"


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
