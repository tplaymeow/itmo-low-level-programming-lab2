#include "sql_ast.h"
#include <stdlib.h>

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
    free(current);
  }
}