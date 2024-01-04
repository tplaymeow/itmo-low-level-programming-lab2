#ifndef ITMO_LOW_LEVEL_PROGRAMMING_LAB2_SQL_AST_H
#define ITMO_LOW_LEVEL_PROGRAMMING_LAB2_SQL_AST_H

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

enum sql_ast_data_type {
  SQL_AST_DATA_TYPE_INTEGER,
  SQL_AST_DATA_TYPE_FLOATING_POINT,
  SQL_AST_DATA_TYPE_BOOLEAN,
  SQL_AST_DATA_TYPE_TEXT
};

struct sql_ast_column_with_type {
  char *name;
  enum sql_ast_data_type type;
};

struct sql_ast_column_with_type_list {
  struct sql_ast_column_with_type item;
  struct sql_ast_column_with_type_list *next;
};

struct sql_ast_column_with_type_list *sql_ast_column_with_type_list_create(
    struct sql_ast_column_with_type item,
    struct sql_ast_column_with_type_list *next);

void sql_ast_column_with_type_list_free(
    struct sql_ast_column_with_type_list *list);

union sql_ast_literal_value {
  int32_t integer;
  double floating_point;
  bool boolean;
  char *text;
};

struct sql_ast_literal {
  enum sql_ast_data_type type;
  union sql_ast_literal_value value;
};

struct sql_ast_literal_list {
  struct sql_ast_literal item;
  struct sql_ast_literal_list *next;
};

struct sql_ast_literal_list *
sql_ast_literal_list_create(struct sql_ast_literal item,
                            struct sql_ast_literal_list *next);

void sql_ast_literal_list_free(struct sql_ast_literal_list *list);

enum sql_ast_operand_type {
  SQL_AST_OPERAND_TYPE_COLUMN,
  SQL_AST_OPERAND_TYPE_LITERAL
};

union sql_ast_operand_value {
  char *column;
  struct sql_ast_literal literal;
};

struct sql_ast_operand {
  enum sql_ast_operand_type type;
  union sql_ast_operand_value value;
};

enum sql_ast_comparison_operator {
  SQL_AST_COMPARISON_OPERATOR_EQUAL,
  SQL_AST_COMPARISON_OPERATOR_NOT_EQUAL,
  SQL_AST_COMPARISON_OPERATOR_GREATER,
  SQL_AST_COMPARISON_OPERATOR_GREATER_OR_EQUAL,
  SQL_AST_COMPARISON_OPERATOR_LESS,
  SQL_AST_COMPARISON_OPERATOR_LESS_OR_EQUAL
};

struct sql_ast_comparison {
  enum sql_ast_comparison_operator operator;
  struct sql_ast_operand left;
  struct sql_ast_operand right;
};

enum sql_ast_logic_binary_operator {
  SQL_AST_LOGIC_BINARY_OPERATOR_AND,
  SQL_AST_LOGIC_BINARY_OPERATOR_OR
};

struct sql_ast_logic {
  enum sql_ast_logic_binary_operator operator;
  struct sql_ast_filter *left;
  struct sql_ast_filter *right;
};

enum sql_ast_filter_type {
  SQL_AST_FILTER_TYPE_ALL,
  SQL_AST_FILTER_TYPE_COMPARISON,
  SQL_AST_FILTER_TYPE_LOGIC
};

union sql_ast_filter_value {
  struct sql_ast_comparison comparison;
  struct sql_ast_logic logic;
};

struct sql_ast_filter {
  enum sql_ast_filter_type type;
  union sql_ast_filter_value value;
};

struct sql_ast_column_with_literal {
  char *name;
  struct sql_ast_literal literal;
};

struct sql_ast_column_with_literal_list {
  struct sql_ast_column_with_literal item;
  struct sql_ast_column_with_literal_list *next;
};

struct sql_ast_column_with_literal_list *
sql_ast_column_with_literal_list_create(
    struct sql_ast_column_with_literal item,
    struct sql_ast_column_with_literal_list *next);

void sql_ast_column_with_literal_list_free(
    struct sql_ast_column_with_literal_list *list);

struct sql_ast_create_statement {
  char *table_name;
  struct sql_ast_column_with_type_list *columns;
};

struct sql_ast_drop_statement {
  char *table_name;
};

struct sql_ast_insert_statement {
  char *table_name;
  struct sql_ast_literal_list *values;
};

struct sql_ast_select_statement {
  char *table_name;
  struct sql_ast_filter filter;
};

struct sql_ast_delete_statement {
  char *table_name;
  struct sql_ast_filter filter;
};

struct sql_ast_update_statement {
  char *table_name;
  struct sql_ast_filter filter;
  struct sql_ast_column_with_literal_list *set;
};

enum sql_ast_statement_type {
  SQL_AST_STATEMENT_TYPE_CREATE,
  SQL_AST_STATEMENT_TYPE_DROP,
  SQL_AST_STATEMENT_TYPE_INSERT,
  SQL_AST_STATEMENT_TYPE_SELECT,
  SQL_AST_STATEMENT_TYPE_DELETE,
  SQL_AST_STATEMENT_TYPE_UPDATE
};

union sql_ast_statement_value {
  struct sql_ast_create_statement create;
  struct sql_ast_drop_statement drop;
  struct sql_ast_insert_statement insert;
  struct sql_ast_select_statement select;
  struct sql_ast_delete_statement delete;
  struct sql_ast_update_statement update;
};

struct sql_ast_statement {
  enum sql_ast_statement_type type;
  union sql_ast_statement_value value;
};

void sql_ast_statement_free(struct sql_ast_statement statement);

#endif // ITMO_LOW_LEVEL_PROGRAMMING_LAB2_SQL_AST_H
