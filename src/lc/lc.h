
#ifndef LC_H
#define LC_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#define MAX_IDENTIFIER 16
#define MAX_SYMBOLS 256
#define MAX_STRINGS 256

enum
{
        TOKEN_EOF,
        TOKEN_NUMBER,
        TOKEN_SYMBOL,
        TOKEN_STRING,
        TOKEN_ADD,
        TOKEN_SUB,
        TOKEN_CAR,
        TOKEN_EQUAL,
        TOKEN_LESS,
        TOKEN_GREATER,
        TOKEN_LBRACKET,
        TOKEN_RBRACKET,
        TOKEN_SEMICOLON,
        TOKEN_COLON,
        __START_OF_KEYWORDS,
        TOKEN_IF=__START_OF_KEYWORDS,
        TOKEN_ELSE,
        TOKEN_WHILE,
        TOKEN_BEGIN,
        TOKEN_END,
        TOKEN_PROCEDURE,
        TOKEN_DEFINE,
        TOKEN_SET,
        TOKEN_AND,
        TOKEN_XOR,
        TOKEN_OR,
        TOKEN_INT,
        TOKEN_CHAR,
        TOKEN_RETURN,
        __END_OF_KEYWORDS,
};

enum
{
        TYPE_INTEGER,
        TYPE_CHAR,
};

typedef struct
{
        char identifier[MAX_IDENTIFIER];
        bool isfunction,allocated;
        union
        {
                struct
                {
                        int argument_indexes[4]; // symbol indexes, max of four args
                        int argument_count;
                } function;

                struct
                {
                        int offset;
                        int basetype;
                        int ptrdepth;
                        int function;
                } variable;
        } as;
} symbol;

typedef struct
{
        char identifier[MAX_IDENTIFIER];
        int type,num;
} token;

token lc_next(FILE *fp);
void lc_program(FILE *fp);
void lc_unget(token tok);
token lc_peek(FILE *fp);

#endif
