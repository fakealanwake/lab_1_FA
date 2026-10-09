#ifndef CONVERT_H
#define CONVERT_H

#include "status.h"

Status convert_number(const char *text, char *shown, const int shown_size, int *base, unsigned long long *value);

#endif
