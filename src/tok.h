#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define new(T) (T*)malloc(sizeof(T))
#define new_list(T,c) (T*)malloc(sizeof(T) * c)
#define delete(x) free(x); x = NULL;
#define forxin(IDE, range) for (size_t IDE = 0; IDE < range; ++IDE)
#define None NULL

enum Type {
	TOKEN_ERROR,
	TOKEN_OPERATOR,
	TOKEN_SEMICOLON,
	TOKEN_GRAMMAR,
	TOKEN_NUMBER,
	TOKEN_STRING_LITERAL,
	TOKEN_IDENTIFIER,
	TOKEN_KEYWORD,
	TOKEN_PREPROCESSOR
};

struct Token {
	char *data;
	enum Type TokType;
};

struct TokenPool {
	size_t token_count;
	struct Token *toks;
};

const char *Keywords[] = {
	"auto", "break", "case", "char", "const", "continue",
	"default", "do", "double", "else", "enum", "extern", "float",
	"for", "goto", "if", "int", "long", "register", "return", "short",
	"signed", "sizeof", "static", "struct", "switch", "typedef", "union",
	"unsigned", "void", "volatile", "while"
};

struct TokenPool tokens = {.token_count = 0, .toks = NULL};

void append_token(struct Token t)
{
	++tokens.token_count;
	struct Token *new_toks = new_list(struct Token, tokens.token_count);
	if (!new_toks)
	{
		printf("buy more ram lol\n");
		exit(1);
	}

	forxin(ti, tokens.token_count - 1)
	{
		if (tokens.toks[ti].data != None)
			new_toks[ti].data = strdup(tokens.toks[ti].data);
		else
			new_toks[ti].data = None;
		new_toks[ti].TokType = tokens.toks[ti].TokType;
		delete(tokens.toks[ti].data);
	}

	if (t.data != None)
		new_toks[tokens.token_count - 1].data = strdup(t.data);
	else
		new_toks[tokens.token_count - 1].data = None;
	new_toks[tokens.token_count - 1].TokType = t.TokType;

	delete(tokens.toks);
	tokens.toks = new_toks;
}

void delete_toks() {
	forxin(ti, tokens.token_count)
	{
		delete(tokens.toks[ti].data);
	}

	delete(tokens.toks);
}

int is_keyword(const char *str)
{
	for (size_t i = 0; i < sizeof(Keywords) / sizeof(Keywords[0]); ++i)
	{
		if (strcmp(str, Keywords[i]) == 0)
		{
			return 1;
		}
	}
	return 0;
}

void tokenise(const char *str)
{
	char buffer[256];
	size_t buffer_index = 0;
	while (*str)
	{
		if (isspace(*str))
		{
			++str;
			continue;
		}
		
		if (isalpha(*str) || *str == '_')
		{
			buffer_index = 0;
			while (isalnum(*str) || *str == '_')
			{
				buffer[buffer_index++] = *str++;
			}
			buffer[buffer_index] = '\0';

			struct Token token;
			if (is_keyword(buffer))
			{
				token.TokType = TOKEN_KEYWORD;
			} else
			{
				token.TokType = TOKEN_IDENTIFIER;
			}
			token.data = strdup(buffer);
			append_token(token);
		}
		else if (isdigit(*str))
		{
			buffer_index = 0;
			while (isdigit(*str))
			{
				buffer[buffer_index++] = *str++;
			}
			buffer[buffer_index] = '\0';

			struct Token token = {.data = strdup(buffer), .TokType = TOKEN_NUMBER};
			append_token(token);
		}
		else if (*str == '"' || *str == '\'')
		{
			char quote = *str++;
			buffer_index = 0;
			while (*str != quote && *str != '\0')
			{
				buffer[buffer_index++] = *str++;
			}
			buffer[buffer_index] = '\0';
			if (*str) ++str;

			struct Token token = {.data = buffer, .TokType = TOKEN_STRING_LITERAL};
			append_token(token);
		}
		else if (*str == '#')
		{
			char end = '\n';
			buffer_index = 0;
			while (*str != end && *str != '\0')
			{
				buffer[buffer_index++] = *str++;
			}
			buffer[buffer_index] = '\0';
			if (*str) ++str;

			struct Token token = {.data = buffer, .TokType = TOKEN_PREPROCESSOR};
			append_token(token);
		}
		else if (*str == ';')
		{
			struct Token token = {.data = strdup(";"), .TokType = TOKEN_SEMICOLON};
			append_token(token);
			++str;
		}
		else if (strchr("<>=?+-/*%^|~", *str))
		{
			buffer[0] = *str++;
			buffer[1] = '\0';

			struct Token token = {.data = strdup(buffer), .TokType = TOKEN_OPERATOR};
			append_token(token);
		}
		else if (strchr("(){}[]", *str))
		{
			buffer[0] = *str++;
			buffer[1] = '\0';

			struct Token token = {.data = strdup(buffer), .TokType = TOKEN_GRAMMAR};
			append_token(token);
		}
		else
		{
			struct Token token = {.data = None, .TokType = TOKEN_ERROR};
			append_token(token);
			printf("Invalid Character: %c\n", *str);
			exit(1);
		}
	}
}

void print_tokens()
{
	forxin(i, tokens.token_count)
	{
		if (tokens.toks[i].data != None)
			printf("Token: %s, Type: %d\n", tokens.toks[i].data, tokens.toks[i].TokType);
		else
			printf("Token: None\n");
	}
}
