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

/**
 * @brief Returns a string whose value is this string, with all leading space removed.
 *
 * @param s The string to be trimmed.
 * @return a string whose value is this string, with all leading space removed, or this string if it
 * has no leading space.
 */
char *ltrim(char *);

/**
 * @brief Returns a string whose value is this string, with all trailing space removed.
 *
 * @param s The string to be trimmed.
 * @return a string whose value is this string, with all trailing space removed, or this string if
 * it has no trailing space.
 */
char *rtrim(char *);

/**
 * @brief Returns a string whose value is this string, with all leading and trailing space removed.
 *
 * @param s The string to be trimmed.
 * @return a string whose value is this string, with all leading and trailing space removed, or this
 * string if it has no leading or trailing space.
 */
char *trim(char *);

/**
 * @brief Helper function to free GError.
 *
 * @param error Pointer to GError to be freed.
 * @return void
 */
void free_gerror(GError **);
#endif
