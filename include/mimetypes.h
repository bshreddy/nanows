#ifndef _MIMETYPES_H
#define _MIMETYPES_H 1

#ifndef MIME_CONF_FILE
#define MIME_CONF_FILE "./etc/mimetypes.conf"
#endif

#ifndef MIME_BUF_SIZE
#define MIME_BUF_SIZE 128
#endif

#ifndef MIME_HTABLE_SIZE
#define MIME_HTABLE_SIZE 128
#endif

#ifndef DEFAULT_MIMETYPE_KEY
#define DEFAULT_MIMETYPE_KEY "*"
#endif

#include <glib.h>

int create_mime_table();
void destroy_mime_table();
const char *get_mimetype_for_ext(const char *, char *);
const char *get_mimetype_for_url(const char *, char *);

// Internal Helper Functions
char *_get_ext_for_url(const char *);
void _mime_htab_key_destroy(gpointer);
void _mime_htab_value_destroy(gpointer);

#endif
