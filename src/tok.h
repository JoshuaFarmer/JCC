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

const char * Keywords[] =
{
	"auto","break","case","char","const","continue",
	"default","do","double","else","enum","extern","float",
	"for","goto","if","int","long","register","return","short",
	"signed","sizeof","static","struct","switch","typedef","union",
	"unsigned","void","volatile","while"
}
