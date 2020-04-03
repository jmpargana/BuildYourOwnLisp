/**********************************************************************
 *
 * This file contians the definitions of the builtin operator functions
 *
 * Author: Joao Pargana
 *
 **********************************************************************/


#include "../include/repl.h"


lval* builtin_def(lenv* e, lval* a) {
    return builtin_var(e, a, "def");
}


lval* builtin_put(lenv* e, lval* a) {
    return builtin_var(e, a, "=");
}


lval* builtin_var(lenv* e, lval* a, char* func) {
    LASSERT_TYPE(func, a, 0, LVAL_QEXPR);

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
        if (strcmp(func, "def") == 0) {
            lenv_def(e, syms->cell[i], a->cell[i+1]);
        }

        if (strcmp(func, "=") == 0) {
            lenv_put(e, syms->cell[i], a->cell[i+1]);
        }
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


lval* builtin_gt(lenv* e, lval* a) {
    return builtin_ord(e, a, ">");
}


lval* builtin_lt(lenv* e, lval* a) {
    return builtin_ord(e, a, "<");
}


lval* builtin_ge(lenv* e, lval* a) {
    return builtin_ord(e, a, ">=");
}


lval* builtin_le(lenv* e, lval* a) {
    return builtin_ord(e, a, "<=");
}


lval* builtin_ord(lenv* e, lval* a, char* op) {
    LASSERT_NUM(op, a, 2);
    LASSERT_TYPE(op, a, 0, LVAL_NUM);
    LASSERT_TYPE(op, a, 1, LVAL_NUM);

    int r;
    if (strcmp(op, ">") == 0) {
        r = (a->cell[0]->num > a->cell[1]->num);
    }

    if (strcmp(op, "<") == 0) {
        r = (a->cell[0]->num < a->cell[1]->num);
    }

    if (strcmp(op, ">=") == 0) {
        r = (a->cell[0]->num >= a->cell[1]->num);
    }
    
    if (strcmp(op, "<=") == 0) {
        r = (a->cell[0]->num <= a->cell[1]->num);
    }

    lval_del(a);
    return lval_num(r);
}


int lval_eq(lval* x, lval* y) {

    /* Different Types are always unequal */
    if (x->type != y->type) { return 0; }

    /* Compare Based upon type */
    switch (x->type) {
        /* Compare Number Values */
        case LVAL_NUM: return (x->num == y->num);

        /* Compare String Values */
        case LVAL_ERR: return (strcmp(x->err, y->err) == 0);
        case LVAL_SYM: return (strcmp(x->sym, y->sym) == 0);
        case LVAL_STR: return (strcmp(x->str, y->str) == 0);

        /* If builtin compare, otherwise compare formals and body */
        case LVAL_FUN:
            if (x->builtin || y->builtin) {
                return x->builtin == y->builtin;
            } else {
                return lval_eq(x->formals, y->formals)
                    && lval_eq(x->body, y->body);
            }

        /* If list compare every individual element */
        case LVAL_QEXPR:
        case LVAL_SEXPR:
            if (x->count != y->count) { return 0; }

            for (int i = 0; i < x->count; i++) {
                /* If any element not equal then whole list not equal */
                if (!lval_eq(x->cell[i], y->cell[i])) { return 0; }
            }
            return 1;
        break;
    }
    return 0;
}


lval* builtin_cmp(lenv* e, lval* a, char* op) {
    LASSERT_NUM(op, a, 2);

    int r;
    if (strcmp(op, "==") == 0) {
        r = lval_eq(a->cell[0], a->cell[1]);
    }

    if (strcmp(op, "!=") == 0) {
        r = !lval_eq(a->cell[0], a->cell[1]);
    }

    lval_del(a);
    return lval_num(r);
}


lval* builtin_eq(lenv* e, lval* a) {
    return builtin_cmp(e, a, "==");
}


lval* builtin_ne(lenv* e, lval* a) {
    return builtin_cmp(e, a, "!=");
}


lval* builtin_print(lenv* e, lval* a) {

    /* Print each argument followed by a space */
    for (int i = 0; i < a->count; i++) {
        lval_print(a->cell[i]); putchar(' ');
    }

    /* Print a newline and delete arguments */
    putchar('\n');
    lval_del(a);

    return lval_sexpr();
}


lval* builtin_error(lenv* e, lval* a) {
    LASSERT_NUM("error", a, 1);
    LASSERT_TYPE("error", a, 0, LVAL_STR);

    /* Construct Error from first argument */
    lval* err = lval_err(a->cell[0]->str);

    /* Delete arguments and return */
    lval_del(a);
    return err;
}
