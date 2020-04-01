/**********************************************************************
 *
 * Main file which links all definitions in the project as well as the 
 * mpc parsing library to the main function and creates an undless
 * loop with the List REPL
 *
 * Author: Joao Pargana
 *
 *
 **********************************************************************/

#include "../include/repl.h"


int main(int argc, char** argv) {

    mpc_parser_t* Number    = mpc_new("number");
    mpc_parser_t* Symbol    = mpc_new("symbol");
    mpc_parser_t* Sexpr     = mpc_new("sexpr");
    mpc_parser_t* Qexpr     = mpc_new("qexpr");
    mpc_parser_t* Expr      = mpc_new("expr");
    mpc_parser_t* Lispy     = mpc_new("lispy");

	mpca_lang(MPCA_LANG_DEFAULT,
		"                                                           \
			number : /-?[0-9]+/ ;                                   \
			symbol : '+' | '-' | '*' | '/' | '^' | '%'              \
                    | \"join\" | \"eval\" | \"list\"                \
                    | \"head\" | \"tail\" ;                         \
			sexpr : '(' <expr>* ')' ;                               \
            qexpr : '{' <expr>* '}' ;                               \
			expr : <number> | <symbol> | <sexpr> | <qexpr> ;        \
			lispy : /^/ <expr>* /$/ ;                               \
		",
		Number, Symbol, Sexpr, Qexpr, Expr, Lispy);

    puts("Lispy Version 0.0.1");
    puts("Press Ctrl+c to Exit\n");

    while (1) {

        char* input = readline("lispy> ");
        add_history(input);

        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Lispy, &r)) {

            lval* x = lval_eval(lval_read(r.output));
            lval_println(x);
            lval_del(x);

            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        free(input);
    }

    mpc_cleanup(6, Number, Symbol, Sexpr, Qexpr, Expr, Lispy);

    return 0;
}
