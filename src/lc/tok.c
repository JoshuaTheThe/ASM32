
#include <lc/lc.h>

char *keyword = "\002IF\004ELSE\005WHILE\005BEGIN\003END\011PROCEDURE\006DEFINE\003SET\003AND\003XOR\002OR\007INTEGER\004CHAR\006RETURN";

static token lookahead = {0};
static bool  has_lookahead = false;

int lc_identifier(token *tok)
{
        char *p = keyword;
        for (int i = 0; i < __END_OF_KEYWORDS; ++i)
        {
                unsigned char len = *p;
                p++;
                if (tok->num == len && !memcmp(tok->identifier, p, len))
                {
                        return __START_OF_KEYWORDS + i;
                }
                p += len;
        }

        return TOKEN_SYMBOL;
}

token lc_actual_next(FILE *fp)
{
        char x;
        do
        {
                x = fgetc(fp);
                if (x == EOF) { return (token){0}; }
        }
        while (isspace(x));

        token tok = {0};
        switch (x)
        {
                case '+': tok.type = TOKEN_ADD; return tok;
                case '-': tok.type = TOKEN_SUB; return tok;
                case '^': tok.type = TOKEN_CAR; return tok;
                case '=': tok.type = TOKEN_EQUAL; return tok;
                case '<': tok.type = TOKEN_LESS; return tok;
                case '>': tok.type = TOKEN_GREATER; return tok;
                case '[': tok.type = TOKEN_LBRACKET; return tok;
                case ']': tok.type = TOKEN_RBRACKET; return tok;
                case '(': tok.type = TOKEN_LBRACKET; return tok;
                case ')': tok.type = TOKEN_RBRACKET; return tok;
                case ';': tok.type = TOKEN_SEMICOLON; return tok;
                case ':': tok.type = TOKEN_COLON; return tok;
        }

        if (isdigit(x))
        {
                while (isdigit(x))
                {
                        tok.num = tok.num * 10 + x - '0';
                        x = fgetc(fp);
                }
                tok.type = TOKEN_NUMBER;
                ungetc(x, fp);
        }
        else if (isalpha(x))
        {
                while (isalpha(x) || isdigit(x) || x == '_')
                {
                        tok.identifier[tok.num++] = toupper(x);
                        x = fgetc(fp);
                        if (tok.num >= 64) break;
                }
                tok.type = lc_identifier(&tok);
                ungetc(x, fp);
        }
        return tok;
}

token lc_next(FILE *fp)
{
        if (has_lookahead)
        {
                has_lookahead = false;
                //printf("tk: type=%d, '%s', %d\n", lookahead.type, lookahead.identifier, lookahead.num);
                return lookahead;
        }

        token tok = lc_actual_next(fp);
        //printf("tk: type=%d, '%s', %d\n", tok.type, tok.identifier, tok.num);
        return tok;
}

void lc_unget(token tok)
{
        assert(!has_lookahead && "double unget");
        lookahead     = tok;
        has_lookahead = true;
}

token lc_peek(FILE *fp)
{
        token tok = lc_next(fp);
        lc_unget(tok);
        return tok;
}
