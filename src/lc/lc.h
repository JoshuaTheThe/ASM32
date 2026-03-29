
#ifndef LC_H
#define LC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_IDENTIFIER 64

enum
{
        TOKEN_EOF,
        TOKEN_NUMBER,
        TOKEN_SYMBOL,
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
        __END_OF_KEYWORDS,
};

typedef struct
{
        char identifier[MAX_IDENTIFIER];
        int type,num;
} token;

token lc_next(FILE *fp);

#endif
