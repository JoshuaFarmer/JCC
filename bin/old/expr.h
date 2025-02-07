#ifndef EXPR_H
#define EXPR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

extern FILE * fo;

enum
{
        TOK_AUTO=0x100,
        TOK_BREAK,
        TOK_CASE,
        TOK_CHAR,
        TOK_CONST,
        TOK_CONTINUE,
        TOK_DEFAULT,
        TOK_DO,
        TOK_DOUBLE,
        TOK_ELSE,
        TOK_ENUM,
        TOK_EXTERN,
        TOK_FLOAT,
        TOK_FOR,
        TOK_GOTO,
        TOK_IF,
        TOK_INT,
        TOK_LONG,
        TOK_REGISTER,
        TOK_RETURN,
        TOK_SHORT,
        TOK_SIGNED,
        TOK_SIZEOF,
        TOK_STATIC,
        TOK_STRUCT,
        TOK_SWITCH,
        TOK_TYPEDEF,
        TOK_UNION,
        TOK_UNSIGNED,
        TOK_VOID,
        TOK_WHILE,
        TOK_IDE,
};

struct KEY
{
        char * text;
        int    tok;
};

extern char * src;
extern int tok;
void next();
void expr();
void cleanup();

typedef struct VAR
{
        int size;
        int bpoff;
        int is_constant;
        int assigned;
        char * name;
        struct VAR * next;
} VAR;

#endif
