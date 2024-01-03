%{
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

extern int yylex();
extern int yyparse();

void yyerror(const char* s);
%}

%code requires {
#include "../sql_ast.h"
}

%union {
	bool boolean_val;
	double floating_val;
	int32_t integer_val;
	char *text_val;
	char *identifier_val;

    enum sql_ast_comparison_operator comparison_operator_val;

    struct sql_ast_statement statement_val;
    struct sql_ast_create_statement create_statement_val;
    struct sql_ast_drop_statement drop_statement_val;
    struct sql_ast_insert_statement insert_statement_val;
    struct sql_ast_select_statement select_statement_val;
    struct sql_ast_delete_statement delete_statement_val;
    struct sql_ast_update_statement update_statement_val;
    struct sql_ast_column_with_type_list *column_with_type_list_val;
    struct sql_ast_column_with_type column_with_type_val;
    enum sql_ast_data_type data_type_val;
    struct sql_ast_literal_list *literal_list_val;
    struct sql_ast_literal literal_val;
    struct sql_ast_operand operand_val;
    struct sql_ast_comparison comparison_val;
    struct sql_ast_logic logic_val;
    struct sql_ast_filter filter_val;
    struct sql_ast_column_with_literal column_with_literal_val;
    struct sql_ast_column_with_literal_list *column_with_literal_list_val;
}

%token<boolean_val> BOOLEAN_VAL
%token<floating_val> FLOATING_VAL
%token<integer_val> INTEGER_VAL
%token<text_val> TEXT_VAL
%token<identifier_val> IDENTIFIER
%token<comparison_operator_val> COMPARISON_OPERATOR
%token CREATE DROP SELECT INSERT DELETE UPDATE TABLE FROM WHERE INTO INTEGER_TYPE FLOATING_TYPE BOOLEAN_TYPE TEXT_TYPE LEFT_BRACKET RIGHT_BRACKET SEMICOLON COMMA AND OR SET ASSIGN

%type<statement_val> statement
%type<create_statement_val> create_statement
%type<drop_statement_val> drop_statement
%type<insert_statement_val> insert_statement
%type<select_statement_val> select_statement
%type<delete_statement_val> delete_statement
%type<update_statement_val> update_statement
%type<column_with_type_list_val> column_with_type_list column_with_type_list_loop
%type<column_with_type_val> column_with_type
%type<data_type_val> data_type
%type<literal_list_val> literal_list literal_list_loop
%type<literal_val> literal
%type<operand_val> operand
%type<comparison_val> comparison
%type<logic_val> logic
%type<filter_val> filter where
%type<column_with_literal_val> column_with_literal
%type<column_with_literal_list_val> column_with_literal_list

%start input

%%

input
    : statement SEMICOLON {
        printf("%s", $1.value.select.table_name);
    }
    ;

statement
    : drop_statement {
        $$ = (struct sql_ast_statement) {
            .type = SQL_AST_STATEMENT_TYPE_DROP,
            .value.drop = $1
        };
    }
    | create_statement {
        $$ = (struct sql_ast_statement) {
            .type = SQL_AST_STATEMENT_TYPE_CREATE,
            .value.create = $1
        };
    }
    | insert_statement {
        $$ = (struct sql_ast_statement) {
            .type = SQL_AST_STATEMENT_TYPE_INSERT,
            .value.insert = $1
        };
    }
    | select_statement {
        $$ = (struct sql_ast_statement) {
            .type = SQL_AST_STATEMENT_TYPE_SELECT,
            .value.select = $1
        };
    }
    | update_statement {
        $$ = (struct sql_ast_statement) {
            .type = SQL_AST_STATEMENT_TYPE_UPDATE,
            .value.update = $1
        };
    }
    | delete_statement {
        $$ = (struct sql_ast_statement) {
            .type = SQL_AST_STATEMENT_TYPE_DELETE,
            .value.delete = $1
        };
    }
    ;

create_statement
    : CREATE TABLE IDENTIFIER column_with_type_list {
        $$ = (struct sql_ast_create_statement) {
            .table_name = $3,
            .columns = $4
        };
    }
    ;

drop_statement
    : DROP TABLE IDENTIFIER {
        $$ = (struct sql_ast_drop_statement) {
            .table_name = $3
        };
    }
    ;

insert_statement
    : INSERT INTO IDENTIFIER literal_list {
        $$ = (struct sql_ast_insert_statement) {
            .table_name = $3,
            .values = $4
        };
    }
    ;

select_statement
    : SELECT FROM IDENTIFIER where {
        $$ = (struct sql_ast_select_statement) {
            .table_name = $3,
            .filter = $4
        };
    }
    ;

update_statement
    : UPDATE IDENTIFIER SET column_with_literal_list where {
        $$ = (struct sql_ast_update_statement) {
            .table_name = $2,
            .filter = $5,
            .set = $4
        };
    }

delete_statement
    : DELETE FROM IDENTIFIER where {
        $$ = (struct sql_ast_delete_statement) {
            .table_name = $3,
            .filter = $4
        };
    }
    ;

column_with_literal_list
    : column_with_literal {
        $$ = sql_ast_column_with_literal_list_create($1, NULL);
    }
    | column_with_literal_list COMMA column_with_literal {
        $$ = sql_ast_column_with_literal_list_create($3, $1);
    }
    ;

column_with_literal
    : IDENTIFIER ASSIGN literal {
        $$ = (struct sql_ast_column_with_literal) {
            .name = $1,
            .literal = $3
        };
    }
    ;

where
    : {
        $$  = (struct sql_ast_filter) {
            .type = SQL_AST_FILTER_TYPE_ALL
        };
    }
    | WHERE filter {$$  = $2;}
    ;

filter
    : comparison {
        $$  = (struct sql_ast_filter) {
            .type = SQL_AST_FILTER_TYPE_COMPARISON,
            .value.comparison = $1
        };
    }
    | logic {
        $$  = (struct sql_ast_filter) {
            .type = SQL_AST_FILTER_TYPE_LOGIC,
            .value.logic = $1
        };
    }
    | LEFT_BRACKET filter RIGHT_BRACKET {$$ = $2;}
    ;

logic
    : filter OR filter {
        struct sql_ast_filter *left = malloc(sizeof(struct sql_ast_filter)); *left = $1;
        struct sql_ast_filter *right = malloc(sizeof(struct sql_ast_filter)); *right = $3;
        $$ = (struct sql_ast_logic) {
            .operator = SQL_AST_LOGIC_BINARY_OPERATOR_OR,
            .left = left, .right = right
        };
    }
    | filter AND filter {
        struct sql_ast_filter *left = malloc(sizeof(struct sql_ast_filter)); *left = $1;
        struct sql_ast_filter *right = malloc(sizeof(struct sql_ast_filter)); *right = $3;
        $$ = (struct sql_ast_logic) {
            .operator = SQL_AST_LOGIC_BINARY_OPERATOR_AND,
            .left = left, .right = right
        };
    }
    ;

comparison
    : operand COMPARISON_OPERATOR operand {
        $$ = (struct sql_ast_comparison) {
            .operator = $2,
            .left = $1,
            .right = $3
        };
    }
    ;

operand
    : IDENTIFIER {
        $$ = (struct sql_ast_operand) {
            .type = SQL_AST_OPERAND_TYPE_COLUMN,
            .value.column = $1
        };
    }
    | literal {
        $$ = (struct sql_ast_operand) {
            .type = SQL_AST_OPERAND_TYPE_LITERAL,
            .value.literal = $1
        };
    }
    ;

literal_list
    : LEFT_BRACKET literal_list_loop RIGHT_BRACKET {$$ = $2;}
    ;

literal_list_loop
    : literal {
        $$ = sql_ast_literal_list_create($1, NULL);
    }
    | literal_list_loop COMMA literal {
        $$ = sql_ast_literal_list_create($3, $1);
    }
    ;

literal
    : INTEGER_VAL {
        $$ = (struct sql_ast_literal) {
            .type = SQL_AST_DATA_TYPE_INTEGER,
            .value.integer = $1
        };
    }
    | FLOATING_VAL {
        $$ = (struct sql_ast_literal) {
            .type = SQL_AST_DATA_TYPE_FLOATING_POINT,
            .value.floating_point = $1
        };
    }
    | BOOLEAN_VAL {
        $$ = (struct sql_ast_literal) {
            .type = SQL_AST_DATA_TYPE_BOOLEAN,
            .value.boolean = $1
        };
    }
    | TEXT_VAL {
        $$ = (struct sql_ast_literal) {
            .type = SQL_AST_DATA_TYPE_TEXT,
            .value.text = $1
        };
    }
    ;

column_with_type_list
    : LEFT_BRACKET column_with_type_list_loop RIGHT_BRACKET {$$ = $2;}
    ;

column_with_type_list_loop
    : column_with_type {
        $$ = sql_ast_column_with_type_list_create($1, NULL);
    }
    | column_with_type_list_loop COMMA column_with_type {
        $$ = sql_ast_column_with_type_list_create($3, $1);
    }
    ;

column_with_type
    : IDENTIFIER data_type {
        $$ = (struct sql_ast_column_with_type) {
            .name = $1,
            .type = $2
        };
    }
    ;

data_type
    : INTEGER_TYPE {$$ = SQL_AST_DATA_TYPE_INTEGER;}
    | FLOATING_TYPE {$$ = SQL_AST_DATA_TYPE_FLOATING_POINT;}
    | BOOLEAN_TYPE {$$ = SQL_AST_DATA_TYPE_BOOLEAN;}
    | TEXT_TYPE {$$ = SQL_AST_DATA_TYPE_TEXT;}
    ;
%%

void yyerror(const char* s) {
	fprintf(stderr, "Parser error: %s\n", s);
	exit(1);
}

int main() {
    yyparse();
    return 0;
}