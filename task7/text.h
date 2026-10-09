#ifndef TEXT_H
#define TEXT_H

#include "status.h"

#include <stddef.h>

Status read_flag(const char *text, char *flag);
Status split_tokens(const char *text, const size_t length, char ***tokens, int *count);
Status merge_tokens(char **first, const int first_count, char **second, const int second_count, char ***out, int *out_count);
Status convert_tokens(char **tokens, const int count, char ***out, int *out_count);
Status join_tokens(char **tokens, const int count, char **text, size_t *length);
void free_tokens(char **tokens, const int count);

#endif
