/**********************************************************************
 *
 * This file contains the declarations for the lisp value reader
 * in the endless loop REPL
 * It reads and formats the data parsing it into a lval tree
 *
 * Author: Joao Pargana
 *
 *
 **********************************************************************/


#include "../include/repl.h"


lval* lval_read_num(mpc_ast_t* t) {
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err("invalid number");
}


lval* lval_read(mpc_ast_t* t) {

    /* If Symbol or Number return conversion to that type */
    if (strstr(t->tag, "number")) { return lval_read_num(t); }
    if (strstr(t->tag, "string")) { return lval_read_str(t); }
    if (strstr(t->tag, "symbol")) { return lval_sym(t->contents); }

    /* If root (>) or sexpr then create empty list */
    lval* x = NULL;

    if (strcmp(t->tag, ">") == 0)   { x = lval_sexpr(); }
    if (strstr(t->tag, "sexpr"))    { x = lval_sexpr(); }
    if (strstr(t->tag, "qexpr"))    { x = lval_qexpr(); }

    /* Fill this list with any valid expression contained within */
    for (int i = 0; i < t->children_num; i++) {

        if (strcmp(t->children[i]->contents, "(") == 0) { continue; }
        if (strcmp(t->children[i]->contents, ")") == 0) { continue; }

        if (strcmp(t->children[i]->contents, "{") == 0) { continue; }
        if (strcmp(t->children[i]->contents, "}") == 0) { continue; }

        if (strcmp(t->children[i]->tag,  "regex") == 0) { continue; }
        if (strstr(t->children[i]->tag, "comment")) { continue; }

        x = lval_add(x, lval_read(t->children[i]));
    }

    return x;
}


lval* lval_read_str(mpc_ast_t* t) {
    /* Cut off the final quote character */
    t->contents[strlen(t->contents)-1] = '\0';
    /* Copy the string missing out the first quote character */
    char* unescaped = malloc(strlen(t->contents+1)+1);
    strcpy(unescaped, t->contents+1);
    /* Pass through the unescape function */
    unescaped = mpcf_unescape(unescaped);
    /* Construct a new lval using the string */
    lval* str = lval_str(unescaped);
    /* Free the string and return */
    free(unescaped);
    return str;
}
