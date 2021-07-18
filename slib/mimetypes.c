#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mimetypes.h"
#include "helpers.h"

GHashTable *_mime_htab = NULL;

int create_mime_table() {
    if(_mime_htab != NULL) return 2;

    FILE *mime_file = NULL;
    char* buf = calloc(MIME_BUF_SIZE, sizeof(char));
    
    if((_mime_htab = g_hash_table_new_full(g_str_hash, g_str_equal, 
                        _mime_htab_key_destroy, _mime_htab_value_destroy)) == NULL) return 0;
    if((mime_file = fopen(MIME_CONF_FILE, "r")) == NULL) return 0;

    while(fgets(buf, MIME_BUF_SIZE, mime_file) != NULL) {
        buf = trim(buf);
        if(buf[0] == '#') continue;
        
        char *key = strdup(strtok(buf, "="));
        char *value = strdup(strtok(NULL, "\n"));
        g_hash_table_insert(_mime_htab, key, value);
    }

    if(mime_file != NULL) fclose(mime_file);
    free(buf);
    return 1;
}

void destroy_mime_table() {
    if(_mime_htab == NULL) return;

    g_hash_table_destroy(_mime_htab);
    _mime_htab = NULL;
    
}

const char* get_mimetype_for_ext(const char *ext, char *mimetype) {
    // TODO: create errno with message "Mime table not found. Call create_mime_table()"
    if(_mime_htab == NULL) return NULL;
    
    // TODO: create errno with message "mimetype is NULL."
    // if(mimetype == NULL) return NULL;
    
    const char* _minetype = NULL;
    if((_minetype = g_hash_table_lookup(_mime_htab, ext)) != NULL) {
        if(mimetype != NULL) strcpy(mimetype, _minetype);
        return _minetype;
    }

    if((_minetype = g_hash_table_lookup(_mime_htab, DEFAULT_MIMETYPE_KEY)) != NULL) {
        if(mimetype != NULL) strcpy(mimetype, _minetype);
        return _minetype;
    }
    
    return NULL;
}

const char *get_mimetype_for_url(const char *url, char *mimetype) {
    return get_mimetype_for_ext(_get_ext_for_url(url), mimetype);
    return NULL;
}

char* _get_ext_for_url(const char *url) {
    char *dot = strrchr(url, '.');
    if(!dot || dot == url) return NULL;
    return dot;
}

void _mime_htab_key_destroy(gpointer data) {
    free(data);
    data = NULL;
}

void _mime_htab_value_destroy(gpointer data){
    free(data);
    data = NULL;
}

// int main() {
//     if(create_mime_table() == 0) return 1;
//     printf("Created Table\n");
    
//     const char *val = get_mimetype_for_ext(".html", NULL);
//     printf(".html = %s\n", val);
//     printf(".html = %s\n", get_mimetype_for_url("/hello.world/index.html", NULL));
//     printf(".png = %s\n", get_mimetype_for_ext(".png", NULL));
//     printf(".png = %s\n", get_mimetype_for_url("/hello.world/index.png", NULL));

//     destroy_mime_table();

//     return 0;
// }
