/**
 * @file include/helpers.h
 * @brief Function Prototypes for commonly used helper functions.
 *
 * This file contains prototypes for commonly used helper functions and eventually any common helper
 * macros, constants, etc.
 *
 * Implemented in `slib/helpers.c`.
 *
 * @author Sai Hemanth Bheemreddy (@SaiHemanthBR)
 * @copyright MIT License; Copyright (c) 2021 Sai Hemanth Bheemreddy
 * @bug No known bugs.
 */

#ifndef _HELPERS_H
#define _HELPERS_H 1

#include <glib.h>

char *ltrim(char *s);
char *rtrim(char *s);
char *trim(char *s);

void free_gerror(GError **);
#endif
