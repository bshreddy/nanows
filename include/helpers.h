#ifndef _HELPERS_H
#define _HELPERS_H 1

#include <glib.h>

char *ltrim(char *s);
char *rtrim(char *s);
char *trim(char *s);

void free_gerror(GError **);
#endif
