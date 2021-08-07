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

/**
 * @brief Defines the default MIME types file.
 */
#ifndef MIME_CONF_FILE
#define MIME_CONF_FILE "./etc/mimetypes.conf"
#endif

/**
 * @brief Defines max size of each record in MIME_CONF_FILE.
 */
#ifndef MIME_BUF_SIZE
#define MIME_BUF_SIZE 128
#endif

/**
 * @brief Defines the key for default MIME type for unknown file types.
 */
#ifndef DEFAULT_MIMETYPE_KEY
#define DEFAULT_MIMETYPE_KEY "*"
#endif

#include <glib.h>

/**
 * @brief Loads MIME types from `MIME_CONF_FILE` and stores them in a hash table.
 *
 * MIME types are stored in `MIME_CONF_FILE` as a series of records. Each record is a key-value pair
 * in the format `<file extension starting with '.'>=<mime type>`. If the line starts with a '#', it
 * is considered a comment. Each line is parsed to extract the key and value and stored in a hash
 * table. If the MIME type already exists in the hash table, the value is overwritten. Hash tables
 * provided by `glib` are used to store MIME types. `g_hash_table_new_full()` is used to create the
 * hash table.
 *
 * If the MIME types are  loaded successfully, the function returns `1` and the MIME types are
 * stored in the `_mime_htab` variable. If the MIME types are already loaded (i.e. `_mime_htab` is
 * not `NULL`), the function returns `2` without performing any action. If the MIME types are not
 * loaded successfully, the function returns `0`.
 *
 * @return On success, returns a non-zero value. On failure, returns 0.
 */
int create_mime_table();

/**
 * @brief Destroys the MIME types hash table and releases memory allocated for it.
 *
 * @return void
 */
void destroy_mime_table();

/**
 * @brief Gets the MIME type for the given file extension.
 *
 * If the MIME type is found in the hash table, the value is copied into `mimetype` and the same is
 * returned. If the MIME type is not found in the hash table, default MIME type (defined by
 * `DEFAULT_MIMETYPE_KEY`) is copied into `mimetype` and the same is returned.
 *
 * `mimetype` can be `NULL`, in this case, the function simply returns the MIME type.
 *
 * If any error occurs, the function returns NULL and `mimetype` is not modified.
 *
 * @param ext The file extension with a leading '.'.
 * @param mimetype Pointer to a string where the MIME type should be copied.
 * @return On success, returns a pointer to string with the MIME type. On failure, returns `NULL`.
 */
const char *get_mimetype_for_ext(const char *, char *);

/**
 * @brief Gets the MIME type for the given URL.
 *
 * Similar to `get_mimetype_for_ext()`, except it accpets a URL as input. The MIME type is extracted
 * from the url and returned.
 *
 * @see get_mimetype_for_ext()
 * @param url The URL path.
 * @param mimetype Pointer to a string where the MIME type should be copied.
 * @return On success, returns a pointer to string with the MIME type. On failure, returns `NULL`.
 */
const char *get_mimetype_for_url(const char *, char *);

// ==============================
// Internal Helper Functions
// ==============================

/**
 * @brief Extracts the file extension from the given URL path.
 *
 * @param url The URL path.
 * @return On success, returns a pointer to string containing extension for the given URL. On
 * failure, returns `NULL`.
 */
char *_get_ext_for_url(const char *);

/**
 * @brief Releases the memory allocated for the given key, called automatically by
 * `g_hash_table_destroy()`.
 *
 * @param data Pointer to key in the hash table.
 * @return void
 */
void _mime_htab_key_destroy(gpointer);

/**
 * @brief Releases the memory allocated for the given value, called automatically by
 * `g_hash_table_destroy()`.
 *
 * @param data Pointer to value in the hash table.
 * @return void
 */
void _mime_htab_value_destroy(gpointer);

#endif
