/**
 * @file slib/mimetypes.c
 * @brief Functions for loading and querying mimetypes.
 *
 * Implements functions defined in `include/mimetypes.h`. Used to load and retrive MIME types for
 * file types.
 *
 * MIME types in `mimetypes.h` are defined as a hash table. The hash table is loaded from the file
 * defined by `MIME_CONF_FILE` macro (defined in `include/mimetypes/h`). MIME types file can be
 * changed by defining `CONF_FILE` macro before `#include "mimetypes.h"`.
 *
 * `MIME_CONF_FILE` can be expected to be a file where each line is a key-value pair with the format
 * <file extension starting with '.'>=<mime type>. If the line starts with a '#', it is considered a
 *  comment. Keys in `MIME_CONF_FILE` must always be file extensions starting with '.'. With the
 * only exception being *default* case that is used to define the default MIME type for all unknown
 * file extensions. And max length of a value is defined by `MIME_BUF_SIZE` macro (defined in
 * `include/mimetypes/h`).
 *
 * The default case key is defined by `DEFAULT_MIMETYPE_KEY` macro (defined in
 * `include/mimetypes/h`). If `DEFAULT_MIMETYPE_KEY` can be changed by defining `CONF_FILE` macro
 * before `#include "mimetypes.h"`. In which case, the key for default MIME type must be changed as
 * well.
 *
 * @author Sai Hemanth Bheemreddy (@SaiHemanthBR)
 * @copyright MIT License; Copyright (c) 2021 Sai Hemanth Bheemreddy
 * @bug No known bugs.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "helpers.h"
#include "mimetypes.h"

GHashTable *_mime_htab = NULL;

int create_mime_table() {
    if (_mime_htab != NULL)
        return 2;

    FILE *mime_file = NULL;
    char *buf = calloc(MIME_BUF_SIZE, sizeof(char));

    if ((_mime_htab = g_hash_table_new_full(g_str_hash, g_str_equal, _mime_htab_key_destroy,
                                            _mime_htab_value_destroy)) == NULL)
        return 0;

    if ((mime_file = fopen(MIME_CONF_FILE, "r")) == NULL)
        return 0;

    while (fgets(buf, MIME_BUF_SIZE, mime_file) != NULL) {
        buf = trim(buf);
        if (buf[0] == '#')
            continue;

        char *key = strdup(strtok(buf, "="));
        char *value = strdup(strtok(NULL, "\n"));
        g_hash_table_insert(_mime_htab, key, value);
    }

    if (mime_file != NULL)
        fclose(mime_file);
    free(buf);
    return 1;
}

void destroy_mime_table() {
    if (_mime_htab == NULL)
        return;

    g_hash_table_destroy(_mime_htab);
    _mime_htab = NULL;
}

const char *get_mimetype_for_ext(const char *ext, char *mimetype) {
    // TODO: create errno with message "Mime table not found. Call create_mime_table()"
    if (_mime_htab == NULL)
        return NULL;

    // TODO: create errno with message "mimetype is NULL."
    // if(mimetype == NULL) return NULL;

    const char *_minetype = NULL;
    if ((_minetype = g_hash_table_lookup(_mime_htab, ext)) != NULL) {
        if (mimetype != NULL)
            strcpy(mimetype, _minetype);
        return _minetype;
    }

    if ((_minetype = g_hash_table_lookup(_mime_htab, DEFAULT_MIMETYPE_KEY)) != NULL) {
        if (mimetype != NULL)
            strcpy(mimetype, _minetype);
        return _minetype;
    }

    return NULL;
}

const char *get_mimetype_for_url(const char *url, char *mimetype) {
    return get_mimetype_for_ext(_get_ext_for_url(url), mimetype);
    return NULL;
}

char *_get_ext_for_url(const char *url) {
    char *dot = strrchr(url, '.');
    if (!dot || dot == url)
        return NULL;
    return dot;
}

void _mime_htab_key_destroy(gpointer data) {
    free(data);
    data = NULL;
}

void _mime_htab_value_destroy(gpointer data) {
    free(data);
    data = NULL;
}
