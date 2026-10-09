#ifndef READ_H
#define READ_H

#include "status.h"

Status read_long(const char *text, long long *number);
Status read_double(const char *text, double *number);
Status read_flag(const char *text, char *flag);

#endif
