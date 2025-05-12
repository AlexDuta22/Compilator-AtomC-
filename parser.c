#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include "parser.h"
#include "lexer.h"
#include "utils.h"

Token *iTk;		// the iterator in the tokens list
Token *consumedTk;		// the last consumed token

void tkerr(const char *fmt,...){
	fprintf(stderr,"error in line %d: ",iTk->line);
	va_list va;
	va_start(va,fmt);
	vfprintf(stderr,fmt,va);
	va_end(va);
	fprintf(stderr,"\n");
	exit(EXIT_FAILURE);
}

bool consume(int code){
	if(iTk->code==code){
		consumedTk=iTk;
		iTk=iTk->next;
		return true;
		}
	return false;
}

//forward declarations of functions, to finf them easier in the code
bool typeBase();
bool unit();
bool structDef();	
bool varDef();
bool arrayDecl();
bool fnDef();
bool fnParam();
bool stm();
bool stmCompound();
bool expr();
bool exprAssign();
bool exprOr();
bool exprAnd();
bool exprEq();
bool exprRel();
bool exprAdd();
bool exprMul();
bool exprCast();
bool exprUnary();
bool exprPostfix();
bool exprPrimary();


// typeBase: TYPE_INT | TYPE_DOUBLE | TYPE_CHAR | STRUCT ID
bool typeBase(){
	Token *start = iTk;
	if(consume(TYPE_INT)){
		return true;
		}
	if(consume(TYPE_DOUBLE)){
		return true;
		}
	if(consume(TYPE_CHAR)){
		return true;
		}
	if(consume(STRUCT)){
		if(consume(ID)){
			return true;
			} 
		else {
			tkerr("error in typeBase: missing ID after STRUCT");
			}
		}
	iTk = start;
	return false;
}

//arrayDecl: LBRACKET INT? RBRACKET
bool arrayDecl(){
	Token *start = iTk;
	if(consume(LBRACKET)){
		consume(INT);
		if(consume(RBRACKET)){
				return true;
			} else {
				tkerr("error in arrayDecl: missing ]");
			}
		}
	iTk = start;
	return false;
}

//varDef: typeBase ID arrayDecl? SEMICOLON
bool varDef(){
	Token *start = iTk;
	if(typeBase()){
		if(consume(ID)){
			arrayDecl();
			if(consume(SEMICOLON)){
				return true;
				}
			else 
				tkerr("error in varDef: missing ; after variable declaration");
			}
		else 
			tkerr("error in varDef: missing ID after typeBase or missing { after structDef");
		}
	iTk = start;
	return false;
}


//structDef: STRUCT ID LACC varDef* RACC SEMICOLON
bool structDef(){
	Token *start = iTk;
	if(consume(STRUCT)){
		if(consume(ID)){
			if(consume(LACC)){
				for(;;){
					if(varDef()){}
					else break;
					}
				if(consume(RACC)){
					if(consume(SEMICOLON)){
						return true;
						}
					else 
						tkerr("error in structDef: missing ; after } in struct definition");
					}
				else 
					tkerr("error in structDef: missing } after varDef list");
				}
			}
		else 
			tkerr("error in structDef: missing ID after STRUCT");
		}
	iTk = start;
	return false;
}

//fnDef: ( typeBase | VOID ) ID LPAR ( fnParam ( COMMA fnParam )* )? RPAR stmCompound
bool fnDef(){
	Token *start = iTk;
	if (typeBase()){
		}
	else if(consume(VOID)){
		}
	else 
	{
		return false;
	}
	if(consume(ID)){
		if(consume(LPAR)){
			if(fnParam()){
				for(;;){
					if(consume(COMMA)){
						if(fnParam()){
						}
						else tkerr("error in fnDef: missing or invalid Parameter after ,");
						}
					else break;
					}
				}
			if(consume(RPAR)){
				if(stmCompound()){
					return true;
					} else {
					tkerr("error in fnDef: missing function body after )");
					}
				}
				else 
					tkerr("error in fnDef: missing ) after fnParam list");
			}
		}
		else{ 
			tkerr("error in fnDef: missing function name after function type");
		}
	iTk = start;
	return false;
}

//fnParam: typeBase ID arrayDecl?
bool fnParam(){
	Token *start = iTk;
	if(typeBase()){
		if(consume(ID)){
			arrayDecl();
			return true;
			}
		else {
			tkerr("error in fnParam: missing ID after typeBase");
		}	
	}
	iTk = start;
	return false;
}

//stm: stmCompound
//| IF LPAR expr RPAR stm ( ELSE stm )? 
//| WHILE LPAR expr RPAR stm
//| RETURN expr? SEMICOLON 
//| expr? SEMICOLON
bool stm(){
	Token *start = iTk;
	if(stmCompound()){
		return true;
	}
	iTk = start;
	if(consume(IF)){
		if(consume(LPAR)){
			if(expr()){
				if(consume(RPAR)){
					bool foundStm=false;
					if(stm()){
						foundStm=true;
					}
					if(consume(ELSE)){
						if(stm())
						{}
						else 
							tkerr("error in stm: missing statement after ELSE");
					}
					if(foundStm){
						return true;
					}
					else 
						tkerr("error in stm: missing statement after IF condition");
				}
				else 
					tkerr("error in stm: missing ) after IF condition");
			}
			else 
				tkerr("error in stm: missing expression after ( in IF statement");
		}
		else 
			tkerr("error in stm: missing ( after IF");
	}
	iTk = start;	
	if(consume(WHILE)){
		if(consume(LPAR)){
			if(expr()){
				if(consume(RPAR)){
					if(stm()){
						return true;
					}
					else 
						tkerr("error in stm: missing statement after WHILE condition");
				}
				else 
					tkerr("error in stm: missing ) after WHILE condition");
			}
			else
				tkerr("error in stm: missing expression after ( in WHILE statement");
		}
		else 
			tkerr("error in stm: missing ( after WHILE");
	}
	iTk = start;
	if(consume(RETURN)){
		expr();
		if(consume(SEMICOLON)){
			return true;
		}
		else 
			tkerr("error in stm: missing ; after RETURN statement");
	}
	iTk = start;
	bool expression = false;
	if(expr()){
		expression = true;
	}
	if(consume(SEMICOLON)){
		return true;
	}
	else 
		if( expression){
			tkerr("error in stm: missing ; after expression");
		}

	iTk = start;
	return false;
}

//stmCompound: LACC ( varDef | stm )* RACC
bool stmCompound(){
	Token *start = iTk;
	if(consume(LACC)){
		for(;;){
			if(varDef()){}
			else 
				if(stm()){}
				else break;
		}
		if(consume(RACC)){
			return true;
		}
		else 
			tkerr("error in stmCompound: missing } after statement list");
	}
	iTk = start;
	return false;
}

//expr: exprAssign
bool expr(){
	Token *start = iTk;
	if(exprAssign()){
		return true;
	}
	iTk = start;
	return false;	
}

//exprAssign: exprUnary ASSIGN exprAssign | exprOr
bool exprAssign(){
	Token *start = iTk;
	if(exprUnary()){
		if(consume(ASSIGN)){
			if(exprAssign()){
				return true;
			}
			else 
				tkerr("error in exprAssign: missing expression after =");
		}
	}
	iTk = start;
	if(exprOr()){
		return true;
	}
	iTk = start;
	return false;
}

//exprOr: exprOr OR exprAnd | exprAnd 

//exprOrPrim: OR exprAnd exprOrPrim | e
//exprOr: exprAnd exprOrPrim
bool exprOrPrim()
{
	Token *start = iTk;
	if(consume(OR)){ // prima alternativă: OR exprAnd exprOrPrim
		if(exprAnd()){
			if(exprOrPrim()){
				return true;
			}
		}
		else tkerr("error in exprOr: missing expression after ||");
	}
	iTk = start;
	return true;
}

bool exprOr()
{
	Token *start = iTk;
	if(exprAnd()){
		if(exprOrPrim()){
			return true;
		}
	}
	iTk = start;
	return false;
}

//exprAnd: exprAnd AND exprEq | exprEq

//exprAndPrim: AND exprEq exprAndPrim | e
//exprAnd: exprEq exprAndPrim
bool exprAndPrim()
{
	Token *start = iTk;
	if(consume(AND)){
		if(exprEq()){
			if(exprAndPrim()){
				return true;
			}
		}
		else tkerr("error in exprAnd: missing expresion after &&");
	}
	iTk = start;
	return true;
}


bool exprAnd()
{
	Token *start = iTk;
	if(exprEq()){
		if(exprAndPrim()){
			return true;
		}
	}
	iTk = start;
	return false;
}

//exprEq: exprEq ( EQUAL | NOTEQ ) exprRel | exprRel

//exprEqPrim:(EQUAL|NOTEQ) exprRel exprEqPrim | e
//exprEq: exprRel exprEqPrim
bool exprEqPrim()
{
	Token *start = iTk;
	if(consume(EQUAL)){
		if(exprRel()){
			if(exprEqPrim()){
				return true;
			}
		}
		else tkerr("error in expreEq: missing expression after == ");
	}
	iTk = start;
	if(consume(NOTEQ)){
		if(exprRel()){
			if(exprEqPrim()){
				return true;
			}
		}
		else tkerr("error in expreEq: missing expression after !=");
	}
	return true;
}

bool exprEq()
{
	Token *start = iTk;
	if(exprRel()){
		if(exprEqPrim()){
			return true;
		}
	}
	iTk = start;
	return false;
}

//exprRel: exprRel ( LESS | LESSEQ | GREATER | GREATEREQ ) exprAdd | exprAdd

//exprRelPrim: (LESS|LESSEQ|GREATER|GREATEREQ) exprAdd exprRelPrim | e
//exprRel: exprAdd exprRelPrim
bool exprRelPrim()
{
	Token *start = iTk;
	if(consume(LESS)){
		if(exprAdd()){
			if(exprRelPrim()){
				return true;
			}
		}
		else tkerr("error in exprRel: missing expression after <");
	}
	iTk = start;
	if(consume(LESSEQ)){
		if(exprAdd()){
			if(exprRelPrim()){
				return true;
			}
		}
		else tkerr("error in exprRel: missing expression after <=");
	}
	iTk = start;
	if(consume(GREATER)){
		if(exprAdd()){
			if(exprRelPrim()){
				return true;
			}
		}
		else tkerr("error in exprRel: missing expression after >");
	}
	iTk = start;
	if(consume(GREATEREQ)){
		if(exprAdd()){
			if(exprRelPrim()){
				return true;
			}
		}
		else tkerr("error in exprRel: missing expression after >=");
	}
	iTk = start;
	return true;
}

bool exprRel(){
	Token *start = iTk;
	if(exprAdd()){
		if(exprRelPrim()){
			return true;
		}
	}
	iTk = start;
	return false;
}

//exprAdd: exprAdd ( ADD | SUB ) exprMul | exprMul

//exprAddPrim:( ADD | SUB ) exprMul exprAddPrim | e
//exprAdd: exprMul exprAddPrim
bool exprAddPrim()
{
	Token *start = iTk;
	if(consume(ADD)){
		if(exprMul()){
			if(exprAddPrim()){
				return true;
			}
		}
		else tkerr("error in exprAdd: missing expression after +\n");
	}

	iTk = start;
	if(consume(SUB)){
		if(exprMul()){
			if(exprAddPrim()){
				return true;
			}
		}
		else tkerr("error in exprAdd: missing expression after -\n");
	}

	iTk = start;
	return true;
}

bool exprAdd(){
	Token *start = iTk;
	if(exprMul()){
		if(exprAddPrim()){
			return true;
		}
	}
	iTk = start;
	return false;
}

//exprMul: exprMul ( MUL | DIV ) exprCast | exprCast

//exprMulPrim:  ( MUL | DIV ) exprCast exprMulPrim | e
//exprMul: exprCast exprMulPrim
bool exprMulPrim()
{
	Token *start = iTk;
	if(consume(MUL)){
		if(exprCast()){
			if(exprMulPrim()){
				return true;
			}
		}
		else tkerr("error in exprMul: missing expression after * \n");
	}

	iTk = start;

	if(consume(DIV)){
		if(exprCast()){
			if(exprMulPrim()){
				return true;
			}
		}
		else tkerr("error in exprMul: missing expression after /\n");
	}

	iTk = start;
	return true;
}


bool exprMul()
{
	Token  *start = iTk;
	if(exprCast()){
		if(exprMulPrim()){
			return true;
		}
	}
	iTk = start;
	return false;
}

//exprCast: LPAR typeBase arrayDecl? RPAR exprCast | exprUnary
bool exprCast(){
	Token *start = iTk;
	if(consume(LPAR)){
		if(typeBase()){
			arrayDecl();
			if(consume(RPAR)){
				if(exprCast()){
					return true;
				}
			}
			else
				tkerr("error in exprCast: missing ) after typeBase");
			}
	}
	iTk = start;
	if(exprUnary()){
		return true;
	}
	iTk = start;
	return false;
}
	
//exprUnary: ( SUB | NOT ) exprUnary | exprPostfix
bool exprUnary(){
	Token *start = iTk;
	if(exprPostfix()){
		return true;
	}

	if(consume(SUB)){
		if(exprUnary()){
			return true;
		}
		else 
			tkerr("error in exprUnary: missing expression after -");
	}
	else {
		iTk = start;
		if(consume(NOT)){
			if(exprUnary()){
				return true;
			}
			else 
				tkerr("error in exprUnary: missing expression after !");
		}
	
	}
	iTk = start;
	return false;
}

//exprPostfix: exprPostfix LBRACKET expr RBRACKET| exprPostfix DOT ID| exprPrimary

//exprPostfixPrim: LBRACKET expr RBRACKET exprPostfixPrim | DOT ID exprPostfixPrim | w
//exprPostfix: exprPrimary exprPostfixPrim
bool exprPostfixPrim()
{
	Token *start = iTk;
	if(consume(LBRACKET)){
		if (expr()){
			if(consume(RBRACKET)){
				if(exprPostfixPrim()){
					return true;
				}
			}
			else tkerr("error in exprpostfix: Missing ] after expression in [ ]\n");
		}
		else tkerr("error in exprpostfix: Missing expression after [\n");
	}
	iTk = start;
	if (consume(DOT)){
		if(consume(ID)){
			if(exprPostfixPrim()){
				return true;
			}
		}
		else tkerr("error in exprpostfix: Missing identifier after .\n");
	}
	iTk = start;
	return true;
	
}

bool exprPostfix()
{
	Token *start = iTk;
	if(exprPrimary()){
		if(exprPostfixPrim()){
			return true;
		}
	}
	iTk = start;
	return false;
}



//exprPrimary: ID ( LPAR ( expr ( COMMA expr )* )? RPAR )?
//| INT | DOUBLE | CHAR | STRING | LPAR expr RPAR
bool exprPrimary(){
	Token *start = iTk;
	if(consume(ID)){
		if(consume(LPAR)){
			if(expr()){
				for(;;){
					if(consume(COMMA)){
						if(expr()){
						}
						else tkerr("error in exprPrimary: missing expression after ,");
						}
					else break;
					}
				if(consume(RPAR)){
					return true;
					}
				else 
					tkerr("error in exprPrimary: missing ) after function call");
				}
                return true; //aici
			}
            return true; //aici
	}
	iTk = start;
	if(consume(INT)){
		return true;
		}
	if(consume(DOUBLE)){
		return true;
		}
	if(consume(CHAR)){
		return true;
		}
	if(consume(STRING)){
		return true;
		}
	if(consume(LPAR)){
		if(expr()){
			if(consume(RPAR)){
				return true;
				}
			else
				tkerr("error in exprPrimary: missing ) after expression");
			}
		else
			tkerr("error in exprPrimary: missing expression after (");
		}
	iTk = start;
	return false;
}

// unit: ( structDef | fnDef | varDef )* END
bool unit(){
	Token *start = iTk;
	for(;;){
		if(structDef()){}
		else if(fnDef()){}
		else if(varDef()){}
		else break;
		}
	if(consume(END)){
		printf("Parsing completed successfully.\n");
		return true;
		}
	iTk = start;
	return false;
}

void parse(Token *tokens){
	iTk=tokens;
	if(!unit())tkerr("syntax error");
}