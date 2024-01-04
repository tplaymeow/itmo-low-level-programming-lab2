#include "sql_ast.h"
#include <stdlib.h>

static void sql_ast_literal_free(struct sql_ast_literal literal) {
  switch (literal.type) {
  case SQL_AST_DATA_TYPE_INTEGER:
  case SQL_AST_DATA_TYPE_FLOATING_POINT:
  case SQL_AST_DATA_TYPE_BOOLEAN:
    break;
  case SQL_AST_DATA_TYPE_TEXT:
    free(literal.value.text);
  }
}

struct sql_ast_column_with_type_list *sql_ast_column_with_type_list_create(
    struct sql_ast_column_with_type item,
    struct sql_ast_column_with_type_list *next) {
  struct sql_ast_column_with_type_list *result =
      malloc(sizeof(struct sql_ast_column_with_type_list));
  result->item = item;
  result->next = next;
  return result;
}

void sql_ast_column_with_type_list_free(
    struct sql_ast_column_with_type_list *list) {
  struct sql_ast_column_with_type_list *next = list;
  while (next != NULL) {
    struct sql_ast_column_with_type_list *current = next;
    next = next->next;

    free(current->item.name);
    free(current);
  }
}

struct sql_ast_literal_list *
sql_ast_literal_list_create(struct sql_ast_literal item,
                            struct sql_ast_literal_list *next) {
  struct sql_ast_literal_list *result =
      malloc(sizeof(struct sql_ast_literal_list));
  result->item = item;
  result->next = next;
  return result;
}

void sql_ast_literal_list_free(struct sql_ast_literal_list *list) {
  struct sql_ast_literal_list *next = list;
  while (next != NULL) {
    struct sql_ast_literal_list *current = next;
    next = next->next;

    sql_ast_literal_free(current->item);
    free(current);
  }
}

struct sql_ast_column_with_literal_list *
sql_ast_column_with_literal_list_create(
    struct sql_ast_column_with_literal item,
    struct sql_ast_column_with_literal_list *next) {
  struct sql_ast_column_with_literal_list *result =
      malloc(sizeof(struct sql_ast_column_with_literal_list));
  result->item = item;
  result->next = next;
  return result;
}

void sql_ast_column_with_literal_list_free(
    struct sql_ast_column_with_literal_list *list) {
  struct sql_ast_column_with_literal_list *next = list;
  while (next != NULL) {
    struct sql_ast_column_with_literal_list *current = next;
    next = next->next;

    sql_ast_literal_free(current->item.literal);
    free(current->item.name);
    free(current);
  }
}

static void sql_ast_operand_free(struct sql_ast_operand operand) {
  switch (operand.type) {
  case SQL_AST_OPERAND_TYPE_COLUMN:
    free(operand.value.column);
    break;
  case SQL_AST_OPERAND_TYPE_LITERAL:
    sql_ast_literal_free(operand.value.literal);
    break;
  }
}

static void sql_ast_filter_free(struct sql_ast_filter filter) {
  switch (filter.type) {
  case SQL_AST_FILTER_TYPE_ALL:
  case SQL_AST_FILTER_TYPE_COMPARISON:
    sql_ast_operand_free(filter.value.comparison.left);
    sql_ast_operand_free(filter.value.comparison.right);
    break;
  case SQL_AST_FILTER_TYPE_LOGIC:
    sql_ast_filter_free(*filter.value.logic.left);
    sql_ast_filter_free(*filter.value.logic.right);
    free(filter.value.logic.left);
    free(filter.value.logic.right);
    break;
  }
}

void sql_ast_statement_free(struct sql_ast_statement statement) {
  switch (statement.type) {
  case SQL_AST_STATEMENT_TYPE_CREATE: {
    free(statement.value.create.table_name);
    sql_ast_column_with_type_list_free(statement.value.create.columns);
  } break;
  case SQL_AST_STATEMENT_TYPE_DROP: {
    free(statement.value.drop.table_name);
  } break;
  case SQL_AST_STATEMENT_TYPE_INSERT: {
    free(statement.value.insert.table_name);
    sql_ast_literal_list_free(statement.value.insert.values);
  } break;
  case SQL_AST_STATEMENT_TYPE_SELECT: {
    free(statement.value.select.table_name);
    sql_ast_filter_free(statement.value.select.filter);
  } break;
  case SQL_AST_STATEMENT_TYPE_DELETE: {
    free(statement.value.delete.table_name);
    sql_ast_filter_free(statement.value.delete.filter);
  } break;
  case SQL_AST_STATEMENT_TYPE_UPDATE: {
    free(statement.value.update.table_name);
    sql_ast_filter_free(statement.value.update.filter);
    sql_ast_column_with_literal_list_free(statement.value.update.set);
  } break;
  }
}