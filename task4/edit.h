#ifndef EDIT_H
#define EDIT_H

#include "status.h"

#include <stddef.h>

Status read_action(const char *text, char *action, int *own_name);
Status make_output_name(const char *input, char **output);
Status change_text(const char *text, const size_t length, const char action, char **result, size_t *result_length);

#endif
