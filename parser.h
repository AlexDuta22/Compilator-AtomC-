#pragma once

#include "lexer.h"

void parse(Token *tokens);
bool consume(int code);

// Function declarations for all syntactic rules
void tkerr(const char *fmt,...);
bool typeBase();
bool unit();
bool structDef();
bool varDef();
bool arrayDecl();
bool fnDef();
bool fnParam();
bool stmCompound();
bool stm();
bool expr();
bool exprAssign();
bool exprOr();
bool exprOrPrim();
bool exprAnd();
bool exprAndPrim();
bool exprEq();
bool exprEqPrim();
bool exprRel();
bool exprRelPrim();
bool exprAdd();
bool exprAddPrim();
bool exprMul();
bool exprMulPrim();
bool exprCast();
bool exprUnary();
bool exprPostfixPrim();
bool exprPostfix();
bool exprPrimary();

