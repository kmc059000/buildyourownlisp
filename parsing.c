#include <stdio.h>
#include <math.h>
#include "mpc.h"

/* Declare  a buffer for user input of size 2048 */
static char input[2048];


int number_of_nodes(mpc_ast_t* t) {
    if (t->children_num == 0) { return 1; }
    if (t->children_num >= 1) {
        int total = 1;
        for (int i = 0; i < t->children_num; i++) {
            total = total + number_of_nodes(t->children[i]);
        }
        return total;
    }
    return 0;
}

long eval_op(long x, char* operator, long y) {
    if(strcmp(operator, "+") == 0) {
        return x + y;
    }
    if(strcmp(operator, "-") == 0) {
        return x - y;
    }
    if(strcmp(operator, "*") == 0) {
        return x * y;
    }
    if(strcmp(operator, "/") == 0) {
        return x / y;
    }
    if(strcmp(operator, "%") == 0) {
        return x % y;
    }
    if(strcmp(operator, "^") == 0) {
        return pow(x, y);
    }
    return 0;
}

long eval(mpc_ast_t* t) {
    if (strstr(t->tag, "number")) {
        return atoi(t->contents);
    }

    char* operator = t->children[1]->contents;

    long x = eval(t->children[2]);

    int i = 3;
    while(strstr(t->children[i]->tag, "expr")) {
        long childrenValue = eval(t->children[i]);
        x = eval_op(x, operator, childrenValue);
        i++;
    }

    return x;
}

int main(int argc, char** argv) {
    mpc_parser_t* Number = mpc_new("number");
    mpc_parser_t* Operator = mpc_new("operator");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* Byol = mpc_new("byol");

    mpca_lang(MPCA_LANG_DEFAULT,
        "\
            number      : /-?[0-9]+\\.?[0-9]*/ ; \
            operator    : '+' | '-' | '*' | '/' | '*' | '%' | '^' ; \
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
            // mpc_ast_print(r.output);
            // mpc_ast_delete(r.output);

            //mpc_ast_t* a = r.output;            
            // int totalNodes = number_of_nodes(a);
            // printf("Total Nodes: %i\n", totalNodes);

            long result = eval(r.output);
            printf("Result: %li\n", result);

            mpc_ast_delete(r.output);

        } else {
            mpc_err_print(r.output);
            mpc_err_delete(r.output);
        }
    }

    mpc_cleanup(4, Number, Operator, Expr, Byol);

    return 0;
}