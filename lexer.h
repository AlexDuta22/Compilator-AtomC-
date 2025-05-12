#pragma once

enum{
	ID
	// keywords
	,TYPE_CHAR,TYPE_DOUBLE,TYPE_INT
	,STRUCT,IF,ELSE,WHILE,RETURN,VOID
	// delimiters
	,COMMA,END,SEMICOLON
	,LBRACKET,RBRACKET,LACC,RACC
	,LPAR,RPAR
	// constants
	,INT,CHAR,DOUBLE,STRING
	// operators
	,ASSIGN,EQUAL
	,NOTEQ,LESS,GREATER,LESSEQ,GREATEREQ
	,ADD,SUB,MUL,DIV,DOT,MOD
	,NOT,AND,OR
	};

typedef struct Token{
	int code;		// ID, TYPE_CHAR, ...
	int line;		// the line from the input file
	union{
		char *text;		// the chars for ID, STRING (dynamically allocated)
		int i;		// the value for INT
		char c;		// the value for CHAR
		double d;		// the value for DOUBLE
		};
	struct Token *next;		// next token in a simple linked list
	}Token;

typedef struct Atom{
	int code;		// the code of the atom
	union{
		char *text;		// the chars for ID, STRING (dynamically allocated)
		int i;		// the value for INT
		char c;		// the value for CHAR
		double d;		// the value for DOUBLE
		};
	struct Atom *next;		// next atom in a simple linked list
	}Atom;

Token *tokenize(const char *pch);
void showTokens(const Token *tokens,FILE *out);
