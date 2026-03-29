
#include <lc/lc.h>

symbol symbols[MAX_SYMBOLS] = {0};

static int lc_allocate(void)
{
        for (int i = 0; i < MAX_SYMBOLS; ++i)
        {
                if (!symbols[i].allocated)
                {
                        memset(&symbols[i], 0, sizeof(symbols[0]));
                        symbols[i].allocated = true;
                        return i;
                }
        }

        assert(0 && "Ran out of symbols");
        return -1;
}

static void lc_body(FILE *fp);

static void lc_expr(FILE *fp)
{
        token tok = lc_next(fp);
        (void)tok;
}

static void lc_stmt(FILE *fp)
{
        token tok = lc_next(fp);
        switch (tok.type)
        {
                case TOKEN_IF:
                        lc_expr(fp);
                        lc_body(fp);
                        if (lc_peek(fp).type == TOKEN_ELSE)
                        {
                                lc_next(fp);
                        }
                        break;
        }
}

static void lc_body(FILE *fp)
{
        token curr = lc_next(fp);
        assert(curr.type == TOKEN_BEGIN && "syntax error");
        while (lc_peek(fp).type != TOKEN_END)
                lc_stmt(fp);
        lc_next(fp);
        assert(lc_next(fp).type == TOKEN_SEMICOLON && "syntax error");
}

static void lc_procedure(FILE *fp)
{
        int me = lc_allocate();
        token name = lc_next(fp);
        assert(name.type == TOKEN_SYMBOL && "syntax error");
        assert(lc_next(fp).type == TOKEN_LBRACKET && "syntax error");
        token current;
        symbols[me].isfunction = true;
        memcpy(symbols[me].identifier, name.identifier, MAX_IDENTIFIER);
        do
        {
                current = lc_next(fp);
                if (current.type == TOKEN_RBRACKET) break;
                assert(current.type == TOKEN_SYMBOL && "syntax error");
                assert(lc_next(fp).type == TOKEN_COLON && "syntax error");
                token type = lc_next(fp);
                while (type.type == TOKEN_CAR)
                        type = lc_next(fp);
                current = lc_next(fp);
        }
        while (current.type == TOKEN_SEMICOLON);
        lc_body(fp);
}

void lc_program(FILE *fp)
{
        token tok;
        do
        {
                tok = lc_next(fp);
                if (tok.type == TOKEN_PROCEDURE)
                {
                        lc_procedure(fp);
                }
        } while (tok.type != TOKEN_EOF);
}
