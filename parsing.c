#include <stdio.h>
#include "mpc.h"

/* Declare  a buffer for user input of size 2048 */
static char input[2048];


int main(int argc, char** argv) {
    mpc_parser_t* Number = mpc_new("number");
    mpc_parser_t* Operator = mpc_new("operator");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* Byol = mpc_new("byol");

    mpca_lang(MPCA_LANG_DEFAULT,
        "\
            number      : /-?[0-9]+\\.?[0-9]*/ ; \
            operator    : '+' | '-' | '*' | '/' | '*' | '%' ; \
            expr        : <number> | '(' <operator> <expr>+ ')' ; \
            byol        : /^/ <operator> <expr>+ /$/ ; \
        ", Number, Operator, Expr, Byol);

    /* Print version and exit information */
    puts("BYOL Version 0.0.0.0.1");
    puts("Ctrl+C to exit\n");



    while (1) {
        fputs("byol> ", stdout);

        fgets(input, 2048, stdin);

        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Byol, &r)) {
            mpc_ast_print(r.output);
            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.output);
            mpc_err_delete(r.output);
        }
    }

    mpc_cleanup(4, Number, Operator, Expr, Byol);

    return 0;
}