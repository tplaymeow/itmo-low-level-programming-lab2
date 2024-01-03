%option noyywrap
%option caseless

%{
#include <stdio.h>
#include <stdbool.h>
#include "parser.h"

void yyerror_lexer(const char* s);
%}

%%

"create" {return CREATE;}
"drop" {return DROP;}
"select" {return SELECT;}
"insert" {return INSERT;}
"delete" {return DELETE;}
"update" {return UPDATE;}
"table" {return TABLE;}
"from" {return FROM;}
"where" {return WHERE;}
"into" {return INTO;}
"set" {return SET;}
"integer" {return INTEGER_TYPE;}
"float" {return FLOATING_TYPE;}
"bool" {return BOOLEAN_TYPE;}
"text" {return TEXT_TYPE;}
"and" {return AND;}
"or" {return OR;}
"=" {return ASSIGN;}
"==" {yylval.comparison_operator_val = SQL_AST_COMPARISON_OPERATOR_EQUAL; return COMPARISON_OPERATOR;}
"!=" {yylval.comparison_operator_val = SQL_AST_COMPARISON_OPERATOR_NOT_EQUAL; return COMPARISON_OPERATOR;}
">" {yylval.comparison_operator_val = SQL_AST_COMPARISON_OPERATOR_GREATER; return COMPARISON_OPERATOR;}
">=" {yylval.comparison_operator_val = SQL_AST_COMPARISON_OPERATOR_GREATER_OR_EQUAL; return COMPARISON_OPERATOR;}
"<" {yylval.comparison_operator_val = SQL_AST_COMPARISON_OPERATOR_LESS; return COMPARISON_OPERATOR;}
"<=" {yylval.comparison_operator_val = SQL_AST_COMPARISON_OPERATOR_LESS_OR_EQUAL; return COMPARISON_OPERATOR;}
"(" {return LEFT_BRACKET;}
")" {return RIGHT_BRACKET;}
";" {return SEMICOLON;}
"," {return COMMA;}
"true" {yylval.boolean_val = true; return BOOLEAN_VAL;}
"false" {yylval.boolean_val = false; return BOOLEAN_VAL;}
[0-9]+\.[0-9]+ 	{yylval.floating_val = atof(yytext); return FLOATING_VAL;}
[0-9]+ {yylval.integer_val = atoi(yytext); return INTEGER_VAL;}
[a-zA-Z_][a-zA-Z0-9_]* {yylval.identifier_val = strdup(yytext); return IDENTIFIER;}
\"([^\\\"]|\\.)*\"  {yylval.text_val = strdup(yytext); return TEXT_VAL;}

[ \t\n] {}

. { yyerror_lexer(yytext); }

%%

void yyerror_lexer(const char* s) {
	fprintf(stderr, "Lexer error. Token: %s\n", s);
	exit(1);
}