/**
 * @file slib/helpers.c
 * @brief Collection of commonly used helper functions.
 *
 * Implements collection of helper functions defined in `include/helpers.h`.
 *
 * @author Sai Hemanth Bheemreddy (@SaiHemanthBR)
 * @copyright MIT License; Copyright (c) 2021 Sai Hemanth Bheemreddy
 * @bug No known bugs.
 */

#include <ctype.h>
#include <string.h>

#include "helpers.h"

char *ltrim(char *s) {
    while (isspace(*s))
        s++;
    return s;
}

char *rtrim(char *s) {
    char *back = s + strlen(s);
    while (isspace(*--back));
    *(back + 1) = '\0';
    return s;
}

char *trim(char *s) { return rtrim(ltrim(s)); }

void free_gerror(GError **error) {
    g_error_free(*error);
    *error = NULL;
}
