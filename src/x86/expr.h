#ifndef EXPR_H
#define EXPR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
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
        TOK_EQ,
        TOK_NEQ,
        TOK_NUM,
        TOK_STR,
};

struct KEY
{
        char * text;
        int    tok;
};

typedef struct STRING
{
        char text[64];
        struct STRING * next;
} STRING;

extern char * src;
extern int tok;
void compiler(char *);

typedef struct
{
        uint8_t data[256];
        uint8_t sp;
} Stack;

typedef struct VAR
{
        int size;
        int bpoff;
        bool con;
        bool assigned;
        char * name;
        struct VAR * next;
} Variable;

#ifdef _expr_
struct KEY keys[]=
{
        {.text="auto",TOK_AUTO},
        {.text="break",TOK_BREAK},
        {.text="case",TOK_CASE},
        {.text="char",TOK_CHAR},
        {.text="const",TOK_CONST},
        {.text="continue",TOK_CONTINUE},
        {.text="default",TOK_DEFAULT},
        {.text="do",TOK_DO},
        {.text="double",TOK_DOUBLE},
        {.text="else",TOK_ELSE},
        {.text="enum",TOK_ENUM},
        {.text="extern",TOK_EXTERN},
        {.text="float",TOK_FLOAT},
        {.text="for",TOK_FOR},
        {.text="goto",TOK_GOTO},
        {.text="if",TOK_IF},
        {.text="int",TOK_INT},
        {.text="long",TOK_LONG},
        {.text="register",TOK_REGISTER},
        {.text="return",TOK_RETURN},
        {.text="short",TOK_SHORT},
        {.text="signed",TOK_SIGNED},
        {.text="sizeof",TOK_SIZEOF},
        {.text="static",TOK_STATIC},
        {.text="struct",TOK_STRUCT},
        {.text="switch",TOK_SWITCH},
        {.text="typedef",TOK_TYPEDEF},
        {.text="union",TOK_UNION},
        {.text="unsigned",TOK_UNSIGNED},
        {.text="void",TOK_VOID},
        {.text="while",TOK_WHILE},
};

void spush(Stack * stck, int x)
{
        stck->data[stck->sp++]=x;
}

int spop(Stack * stck)
{
        return stck->data[--stck->sp];
}

int stop(Stack * stck)
{
        return stck->data[stck->sp-1];
}
#endif

#endif
