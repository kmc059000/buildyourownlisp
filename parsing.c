#include <stdio.h>
#include <math.h>
#include "mpc.h"

typedef struct {
    int type;
    long num;
    char* err;
    char* sym;
    //number of lval in cell
    int count;
    struct lval** cell;
} lval;

enum { LVAL_NUM, LVAL_ERR, LVAL_SYM, LVAL_SEXPR };

lval* lval_num(long x) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_NUM;
    v->num = x;
    return v;
}

lval* lval_err(char* x) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_ERR;
    v->err = malloc(strlen(x) + 1);
    strcpy(v->err, x);
    return v;
}

lval* lval_sym(char* s) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_SYM;
    v->sym = malloc(strlen(s) + 1);
    strcpy(v->sym, s);
    return v;
}

lval* lval_sexpr(void) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_SEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

void lval_del(lval* v) {
    switch (v->type) {
        case LVAL_NUM: break;
        case LVAL_ERR:
            free(v->err);
            break;
        case LVAL_SYM:
            free(v->sym);
            break;
        case LVAL_SEXPR:
            for(int i = 0; i < v->count; i++) {
                lval_del(v->cell[i]);
            }
            free(v->cell);
            break;
    }

    free(v);
}

void lval_print(lval v) {
    switch (v.type) {
        case LVAL_NUM:
            printf("%li", v.num);
            break;
        case LVAL_ERR:
            switch(v.err) {
                case LERR_DIV_ZERO:
                    printf("Error: Division by Zero!");
                    break;
                case LERR_BAD_OP:
                    printf("Error: Bad Operator!");
                    break;
                case LERR_BAD_NUM:
                    printf("Error: Bad Number!");
                    break;
            }
        break;
    }
}

void lval_println(lval v) {
    lval_print(v); putchar('\n');
}

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

lval eval_op(lval x, char* operator, lval y) {
    if(x.type == LVAL_ERR) { return x;}
    if(y.type == LVAL_ERR) { return y;}

    long xx = x.num;
    long yy = y.num;
    long opResult;

    if(strcmp(operator, "+") == 0) {
        return lval_num(xx + yy);
    }
    if(strcmp(operator, "-") == 0) {
        return lval_num(xx - yy);
    }
    if(strcmp(operator, "*") == 0) {
        return lval_num(xx * yy);
    }
    if(strcmp(operator, "/") == 0) {
        return yy == 0
            ? lval_err(LERR_DIV_ZERO)
            : lval_num(xx / yy);
    }
    if(strcmp(operator, "%") == 0) {
        return lval_num(xx % yy);
    }
    if(strcmp(operator, "^") == 0) {
        return lval_num(pow(xx, yy));
    }
    if(strcmp(operator, "min") == 0) {
        opResult = xx < yy ? xx : yy;
        return lval_num(opResult);
    }
    if(strcmp(operator, "max") == 0) {
        opResult = xx > yy ? xx : yy;
        return lval_num(opResult);
    }
    return lval_err(LERR_BAD_OP);
}

lval eval_op_single_expr(lval x, char* operator) {
    if(strcmp(operator, "-") == 0) {
        return lval_num(-1 * x.num);
    }

    return x;
}

lval eval(mpc_ast_t* t) {
    if (strstr(t->tag, "number")) {
        errno = 0;
        long x = strtol(t->contents, NULL, 10);
        return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
    }

    char* operator = t->children[1]->contents;

    lval x = eval(t->children[2]);

    int i = 3;
    while(strstr(t->children[i]->tag, "expr")) {
        lval childrenValue = eval(t->children[i]);
        x = eval_op(x, operator, childrenValue);
        i++;
    }

    //if only one number/expr in this expr, then apply operator to single value as necessary
    if (i == 3) {
        x = eval_op_single_expr(x, operator);
    }

    return x;
}

int main(int argc, char** argv) {
    mpc_parser_t* Number = mpc_new("number");
    mpc_parser_t* Symbol = mpc_new("symbol");
    mpc_parser_t* Sexpr = mpc_new("sexpr");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* Byol = mpc_new("byol");

    mpca_lang(MPCA_LANG_DEFAULT,
        "\
            number      : /-?[0-9]+\\.?[0-9]*/ ; \
            symbol      : '+' | '-' | '*' | '/' | '*' | '%' | '^' | \"min\" | \"max\"  ; \
            sexpr       : '(' <expr>* ')' ; \
            expr        : <number> | <symbol> | <sexpr> ; \
            byol        : /^/ <expr>* /$/ ; \
        ", Number, Symbol, Sexpr, Expr, Byol);

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

            lval result = eval(r.output);
            lval_println(result);

            mpc_ast_delete(r.output);

        } else {
            mpc_err_print(r.output);
            mpc_err_delete(r.output);
        }
    }

    mpc_cleanup(4, Number, Symbol, Sexpr, Expr, Byol);

    return 0;
}