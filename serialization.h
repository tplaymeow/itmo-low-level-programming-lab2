#ifndef ITMO_LOW_LEVEL_PROGRAMMING_LAB2_SERIALIZATION_H
#define ITMO_LOW_LEVEL_PROGRAMMING_LAB2_SERIALIZATION_H

#include "sql_ast.h"
#include <stdio.h>

struct serialization_context;

struct serialization_context *serialization_context_create();

void serialization_context_destroy(struct serialization_context *context);

void serialization_context_set_file(struct serialization_context *context,
                                    FILE *file);

void serialize(struct serialization_context *context,
               struct sql_ast_statement statement);

#endif // ITMO_LOW_LEVEL_PROGRAMMING_LAB2_SERIALIZATION_H
