/**
 * @file include/mimetypes.h
 * @brief Function Prototypes for loading and querying MIME types.
 *
 * This file contains function prototypes to load MIME types for different file extensions from
 * etc/mimetypes.conf and retrive MIME type for different file type or url.
 *
 * Implemented in slib/mimetypes.c
 *
 * @author Sai Hemanth Bheemreddy (@SaiHemanthBR)
 * @copyright MIT License; Copyright (c) 2021 Sai Hemanth Bheemreddy
 * @bug No known bugs.
 */

#ifndef _MIMETYPES_H
#define _MIMETYPES_H 1

#ifndef MIME_CONF_FILE
#define MIME_CONF_FILE "./etc/mimetypes.conf"
#endif

#ifndef MIME_BUF_SIZE
#define MIME_BUF_SIZE 128
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
