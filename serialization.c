#include "serialization.h"
#include <stdio.h>
#include <stdlib.h>

struct serialization_context {
  FILE *file;
  size_t space_depth;
};

struct serialization_context *serialization_context_create() {
  struct serialization_context *result =
      malloc(sizeof(struct serialization_context));
  result->file = stdout;
  result->space_depth = 0;
  return result;
}

void serialization_context_destroy(struct serialization_context *context) {
  free(context);
}

void serialization_context_set_file(struct serialization_context *context,
                                    FILE *file) {
  context->file = file;
}

static void print_spaces(struct serialization_context *context) {
  for (size_t i = 0; i < context->space_depth; i++)
    fprintf(context->file, "  ");
}

// JSON Number serialization

static void serialize_int32_t_field(struct serialization_context *context,
                                    const char *name, int32_t value,
                                    bool is_last) {
  print_spaces(context);
  fprintf(context->file, "\"%s\": %" PRId32 "%s\n", name, value,
          is_last ? "" : ",");
}

static void serialize_double_field(struct serialization_context *context,
                                   const char *name, double value,
                                   bool is_last) {
  print_spaces(context);
  fprintf(context->file, "\"%s\": %f%s\n", name, value, is_last ? "" : ",");
}

// JSON String serialization

static void serialize_string_field(struct serialization_context *context,
                                   const char *name, const char *value,
                                   bool is_last) {
  print_spaces(context);
  fprintf(context->file, "\"%s\": \"%s\"%s\n", name, value, is_last ? "" : ",");
}

// JSON Boolean serialization

static void serialize_boolean_field(struct serialization_context *context,
                                    const char *name, bool value,
                                    bool is_last) {
  print_spaces(context);
  fprintf(context->file, "\"%s\": %s%s\n", name, value ? "true" : "false",
          is_last ? "" : ",");
}

// JSON Object serialization

static void serialize_obj_begin(struct serialization_context *context) {
  print_spaces(context);
  fprintf(context->file, "{\n");
  context->space_depth += 1;
}

static void serialize_obj_field_begin(struct serialization_context *context,
                                      const char *name) {
  print_spaces(context);
  fprintf(context->file, "\"%s\": {\n", name);
  context->space_depth += 1;
}

static void serialize_obj_end(struct serialization_context *context,
                              bool is_last) {
  context->space_depth -= 1;
  print_spaces(context);
  fprintf(context->file, "}%s\n", is_last ? "" : ",");
}

// JSON Array serialization

static void serialize_array_begin(struct serialization_context *context) {
  print_spaces(context);
  fprintf(context->file, "[\n");
  context->space_depth += 1;
}

static void serialize_array_field_begin(struct serialization_context *context,
                                        const char *name) {
  print_spaces(context);
  fprintf(context->file, "\"%s\": [\n", name);
  context->space_depth += 1;
}

static void serialize_array_end(struct serialization_context *context,
                                bool is_last) {
  context->space_depth -= 1;
  print_spaces(context);
  fprintf(context->file, "]%s\n", is_last ? "" : ",");
}

const char *data_type_to_string[] = {[SQL_AST_DATA_TYPE_INTEGER] = "integer",
                                     [SQL_AST_DATA_TYPE_FLOATING_POINT] =
                                         "float",
                                     [SQL_AST_DATA_TYPE_BOOLEAN] = "bool",
                                     [SQL_AST_DATA_TYPE_TEXT] = "text"};

const char *comparison_operator_to_string[] = {
    [SQL_AST_COMPARISON_OPERATOR_EQUAL] = "EQUAL",
    [SQL_AST_COMPARISON_OPERATOR_NOT_EQUAL] = "NOT_EQUAL",
    [SQL_AST_COMPARISON_OPERATOR_GREATER] = "GREATER",
    [SQL_AST_COMPARISON_OPERATOR_GREATER_OR_EQUAL] = "GREATER_OR_EQUAL",
    [SQL_AST_COMPARISON_OPERATOR_LESS] = "LESS",
    [SQL_AST_COMPARISON_OPERATOR_LESS_OR_EQUAL] = "LESS_OR_EQUAL"};

const char *logic_operator_to_string[] = {
    [SQL_AST_LOGIC_BINARY_OPERATOR_AND] = "AND",
    [SQL_AST_LOGIC_BINARY_OPERATOR_OR] = "OR",
};

static void serialize_literal_inner(struct serialization_context *context,
                                    struct sql_ast_literal literal) {
  serialize_string_field(context, "type", data_type_to_string[literal.type],
                         false);
  switch (literal.type) {
  case SQL_AST_DATA_TYPE_INTEGER:
    serialize_int32_t_field(context, "value", literal.value.integer, true);
    break;
  case SQL_AST_DATA_TYPE_FLOATING_POINT:
    serialize_double_field(context, "value", literal.value.floating_point,
                           true);
    break;
  case SQL_AST_DATA_TYPE_BOOLEAN:
    serialize_boolean_field(context, "value", literal.value.boolean, true);
    break;
  case SQL_AST_DATA_TYPE_TEXT:
    serialize_string_field(context, "value", literal.value.text, true);
    break;
  default:
    break;
  }
}

static void serialize_literal(struct serialization_context *context,
                              struct sql_ast_literal literal, bool is_last) {
  serialize_obj_begin(context);
  serialize_literal_inner(context, literal);
  serialize_obj_end(context, is_last);
}

static void serialize_literal_field(struct serialization_context *context,
                                    const char *name,
                                    struct sql_ast_literal literal,
                                    bool is_last) {
  serialize_obj_field_begin(context, name);
  serialize_literal_inner(context, literal);
  serialize_obj_end(context, is_last);
}

static void serialize_operand(struct serialization_context *context,
                              const char *name, struct sql_ast_operand operand,
                              bool is_last) {
  serialize_obj_field_begin(context, name);
  switch (operand.type) {
  case SQL_AST_OPERAND_TYPE_LITERAL:
    serialize_literal_field(context, "literal", operand.value.literal, true);
    break;
  case SQL_AST_OPERAND_TYPE_COLUMN:
    serialize_obj_field_begin(context, "column");
    serialize_string_field(context, "column_name", operand.value.column, true);
    serialize_obj_end(context, true);
    break;
  default:
    break;
  }
  serialize_obj_end(context, is_last);
}

static void serialize_text_operand(struct serialization_context *context,
                                   const char *name,
                                   struct sql_ast_text_operand operand,
                                   bool is_last) {
  serialize_obj_field_begin(context, name);
  switch (operand.type) {
  case SQL_AST_OPERAND_TYPE_LITERAL:
    serialize_obj_field_begin(context, "literal");
    serialize_string_field(context, "type", "text", false);
    serialize_string_field(context, "value", operand.value.literal, true);
    serialize_obj_end(context, true);
    break;
  case SQL_AST_OPERAND_TYPE_COLUMN:
    serialize_obj_field_begin(context, "column");
    serialize_string_field(context, "column_name", operand.value.column, true);
    serialize_obj_end(context, true);
    break;
  default:
    break;
  }
  serialize_obj_end(context, is_last);
}

static void serialize_filter(struct serialization_context *context,
                             const char *name, struct sql_ast_filter filter,
                             bool is_last) {
  switch (filter.type) {
  case SQL_AST_FILTER_TYPE_ALL:
    break;
  case SQL_AST_FILTER_TYPE_COMPARISON: {
    serialize_obj_field_begin(context, name);
    serialize_obj_field_begin(context, "comparison");
      serialize_string_field(context, "operator", comparison_operator_to_string[filter.value.comparison.operator],
                             false);
      serialize_operand(context, "left", filter.value.comparison.left, false);
      serialize_operand(context, "right", filter.value.comparison.right, true);
      serialize_obj_end(context, true);
      serialize_obj_end(context, is_last);
  } break;
  case SQL_AST_FILTER_TYPE_CONTAINS: {
    serialize_obj_field_begin(context, name);
    serialize_obj_field_begin(context, "contains");
    serialize_text_operand(context, "left", filter.value.contains.left, false);
    serialize_text_operand(context, "right", filter.value.contains.right, true);
    serialize_obj_end(context, true);
    serialize_obj_end(context, is_last);
  } break;
  case SQL_AST_FILTER_TYPE_LOGIC: {
    serialize_obj_field_begin(context, name);
    serialize_obj_field_begin(context, "logic");
      serialize_string_field(context, "operator", logic_operator_to_string[filter.value.logic.operator], false);
      serialize_filter(context, "left", *filter.value.logic.left, false);
      serialize_filter(context, "right", *filter.value.logic.right, true);
      serialize_obj_end(context, true);
      serialize_obj_end(context, is_last);
  } break;
  default:
    break;
  }
}

static void
serialize_select_statement(struct serialization_context *context,
                           struct sql_ast_select_statement statement) {
  serialize_obj_begin(context);
  serialize_obj_field_begin(context, "select");
  serialize_string_field(context, "table_name", statement.table_name,
                         !statement.join.has_value &&
                             statement.filter.type == SQL_AST_FILTER_TYPE_ALL);
  if (statement.join.has_value) {
    serialize_obj_field_begin(context, "join");
    serialize_string_field(context, "join_table",
                           statement.join.value.join_table, false);
    serialize_string_field(context, "table_column",
                           statement.join.value.table_column, false);
    serialize_string_field(context, "join_table_column",
                           statement.join.value.join_table_column, true);
    serialize_obj_end(context,
                      statement.filter.type == SQL_AST_FILTER_TYPE_ALL);
  }
  serialize_filter(context, "filter", statement.filter, true);
  serialize_obj_end(context, true);
  serialize_obj_end(context, true);
}

static void
serialize_delete_statement(struct serialization_context *context,
                           struct sql_ast_delete_statement statement) {
  serialize_obj_begin(context);
  serialize_obj_field_begin(context, "delete");
  serialize_string_field(context, "table_name", statement.table_name,
                         statement.filter.type == SQL_AST_FILTER_TYPE_ALL);
  serialize_filter(context, "filter", statement.filter, true);
  serialize_obj_end(context, true);
  serialize_obj_end(context, true);
}

static void
serialize_create_statement(struct serialization_context *context,
                           struct sql_ast_create_statement statement) {
  serialize_obj_begin(context);
  serialize_obj_field_begin(context, "create");
  serialize_string_field(context, "table_name", statement.table_name, false);
  serialize_array_field_begin(context, "columns");
  for (struct sql_ast_column_with_type_list *list = statement.columns;
       list != NULL; list = list->next) {
    serialize_obj_begin(context);
    serialize_string_field(context, "name", list->item.name, false);
    serialize_string_field(context, "type",
                           data_type_to_string[list->item.type], true);
    serialize_obj_end(context, list->next == NULL);
  }
  serialize_array_end(context, true);
  serialize_obj_end(context, true);
  serialize_obj_end(context, true);
}

static void serialize_drop_statement(struct serialization_context *context,
                                     struct sql_ast_drop_statement statement) {
  serialize_obj_begin(context);
  serialize_obj_field_begin(context, "drop");
  serialize_string_field(context, "table_name", statement.table_name, true);
  serialize_obj_end(context, true);
  serialize_obj_end(context, true);
}

static void
serialize_insert_statement(struct serialization_context *context,
                           struct sql_ast_insert_statement statement) {
  serialize_obj_begin(context);
  serialize_obj_field_begin(context, "insert");
  serialize_string_field(context, "table_name", statement.table_name, false);
  serialize_array_field_begin(context, "values");
  for (struct sql_ast_literal_list *list = statement.values; list != NULL;
       list = list->next) {
    serialize_literal(context, list->item, list->next == NULL);
  }
  serialize_array_end(context, true);
  serialize_obj_end(context, true);
  serialize_obj_end(context, true);
}

static void
serialize_update_statement(struct serialization_context *context,
                           struct sql_ast_update_statement statement) {
  serialize_obj_begin(context);
  serialize_obj_field_begin(context, "update");
  serialize_string_field(context, "table_name", statement.table_name, false);
  serialize_filter(context, "filter", statement.filter, false);
  serialize_array_field_begin(context, "values");
  for (struct sql_ast_column_with_literal_list *list = statement.set;
       list != NULL; list = list->next) {
    serialize_obj_begin(context);
    serialize_string_field(context, "name", list->item.name, false);
    serialize_literal_field(context, "literal", list->item.literal, true);
    serialize_obj_end(context, list->next == NULL);
  }
  serialize_array_end(context, true);
  serialize_obj_end(context, true);
  serialize_obj_end(context, true);
}

void serialize(struct serialization_context *context,
               struct sql_ast_statement statement) {
  switch (statement.type) {
  case SQL_AST_STATEMENT_TYPE_CREATE:
    serialize_create_statement(context, statement.value.create);
    break;
  case SQL_AST_STATEMENT_TYPE_DELETE:
    serialize_delete_statement(context, statement.value.delete);
    break;
  case SQL_AST_STATEMENT_TYPE_DROP:
    serialize_drop_statement(context, statement.value.drop);
    break;
  case SQL_AST_STATEMENT_TYPE_INSERT:
    serialize_insert_statement(context, statement.value.insert);
    break;
  case SQL_AST_STATEMENT_TYPE_SELECT:
    serialize_select_statement(context, statement.value.select);
    break;
  case SQL_AST_STATEMENT_TYPE_UPDATE:
    serialize_update_statement(context, statement.value.update);
    break;
  default:
    break;
  }
}