/**********************************************************************
 *
 * This file contains the definitions of the printer functions
 * for the lval type
 * After evaluating the resulting value or error
 * print it to the user
 *
 * Author: Joao Pargana
 *
 *
 **********************************************************************/


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
        case LVAL_STR: lval_print_str(v); break;
        case LVAL_SEXPR: lval_expr_print(v, '(', ')'); break;
        case LVAL_QEXPR: lval_expr_print(v, '{', '}'); break;
        case LVAL_FUN: 
                         
            if (v->builtin) {
                printf("<function>"); break;
            } else {
                printf("(\\ "); lval_print(v->formals);
                putchar(' '); lval_print(v->body); putchar(')');
            }
            break;
    }
}


void lval_print_str(lval* v) {
    /* Make a Copy of the string */
    char* escaped = malloc(strlen(v->str) + 1);
    strcpy(escaped, v->str);
    /* Pass it through the escape function */
    escaped = mpcf_escape(escaped);
    /* Print if between " characters */
    printf("\"%s\"", escaped);
    /* free the copied string */
    free(escaped);
}


/* Just append newline to lisp value interpreted */
void lval_println(lval* v) { lval_print(v); putchar('\n'); }


char* ltype_name(int t) {
    switch (t) {
        case LVAL_FUN: return "Function";
        case LVAL_NUM: return "Number";
        case LVAL_ERR: return "Error";
        case LVAL_SYM: return "Symbol";
        case LVAL_STR: return "String";
        case LVAL_SEXPR: return "S-Expression";
        case LVAL_QEXPR: return "Q-Expression";
        default: return "Unknown";
    }
}

