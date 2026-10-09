#include "read.h"
#include <math.h>
#include <stdlib.h>

// read_double переводит строку в double.
// strtod пишет в end адрес первого символа, который не вошёл в число.
Status read_double(const char *text, double *number) {
    char *end = NULL;
    double value;

    if (text == NULL || number == NULL || text[0] == '\0') {
        return BAD_ARG;
    }
    value = strtod(text, &end);
    // end == text — числа нет; *end != '\0' — после числа остался мусор;
    // isfinite отвергает бесконечность и NaN
    if (end == text || *end != '\0' || !isfinite(value)) {
        return BAD_ARG;
    }
    *number = value;
    return OK;
}
