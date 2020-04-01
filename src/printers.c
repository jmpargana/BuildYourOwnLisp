#include "../include/repl.h"



void lval_expr_print(lval* v, char open, char close) {
    putchar(open);

    for (int i = 0; i < v->count; i++) {

        /* Print value contained within */
        lval_print(v->cell[i]);

        /* Don't print trailing space if last element */
        if (i != (v->count-1)) {
            putchar(' ');
        }
    }

    putchar(close);
}


void lval_print(lval* v) {
    switch (v->type) {
        case LVAL_NUM: printf("%li", v->num); break;
        case LVAL_ERR: printf("Error: %s", v->err); break;
        case LVAL_SYM: printf("%s", v->sym); break;
        case LVAL_SEXPR: lval_expr_print(v, '(', ')'); break;
    }
}


/* Just append newline to lisp value interpreted */
void lval_println(lval* v) { lval_print(v); putchar('\n'); }

