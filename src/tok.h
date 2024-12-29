#include<stdio.h>
#include<stdlib.h>

enum Type
{
	TOKEN_ERROR,
	TOKEN_OPERATOR,
	TOKEN_SEMICOLON,
	TOKEN_GRAMMAR,
	TOKEN_NUMBER,
	TOKEN_STRING_LITERAL,
	TOKEN_IDENTIFIER,
	TOKEN_KEYWORD
};

struct Token
{
	const char * data;
	enum Type TokType;
};
