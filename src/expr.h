#pragma once
#include "tok.h"

typedef struct Expr {
	enum { BIN_OP, LITERAL, IDENTIFIER } type;
	union {
		struct {
			struct Expr* left;
			struct Token* operator;
			struct Expr* right;
		} binOp;
		struct Token* literal;
		struct Token* identifier;
	};
} Expr_t;
