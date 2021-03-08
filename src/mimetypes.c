#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define __USE_GNU 1
#include <search.h>

#include "mimetypes.h"

struct hsearch_data *_mime_htab;

int create_mime_table() {
    if(_mime_htab != NULL) return 1;

    FILE *mime_file = NULL;
    ENTRY entry, *ent;
    _mime_htab = malloc(sizeof(struct hsearch_data));

    char buf[MIME_BUF_SIZE];
    
    if(!hcreate_r(MIME_HTABLE_SIZE, _mime_htab)) return 0;
    if((mime_file = fopen(MIME_CONF_FILE, "r")) == NULL) return 0;

    while(fgets(buf, MIME_BUF_SIZE, mime_file) != NULL) {
        entry.key = strdup(strtok(buf, "="));
        entry.data = (void*) strdup(strtok(NULL, "\n"));

        if(!hsearch_r(entry, ENTER, &ent, _mime_htab)) return 0;
    }

    if(mime_file != NULL) fclose(mime_file);
    return 1;
}

void destroy_mime_table() {
    if(_mime_htab == NULL) return;

    hdestroy_r(_mime_htab);
    free(_mime_htab);
    _mime_htab = NULL;
}

const char* get_mimetype_for_ext(const char *ext, char *mimetype) {
    // TODO: create errno with message "Mime table not found. Call create_mime_table()"
    if(_mime_htab == NULL) return NULL;
    
    // TODO: create errno with message "mimetype is NULL."
    // if(mimetype == NULL) return NULL;
    
    ENTRY entry, *ent;
    if(ext != NULL) {
        entry.key = (char*) ext;
        if(hsearch_r(entry, FIND, &ent, _mime_htab)) {
            if(mimetype != NULL) strcpy(mimetype, (char*)(ent->data));
            return (const char*)(ent->data);
        }
    }

    entry.key = DEFAULT_MIMETYPE_KEY;
    if(hsearch_r(entry, FIND, &ent, _mime_htab)) {
        if(mimetype != NULL) strcpy(mimetype, (char *)(ent->data));
        return (const char*)(ent->data);
    }
    
    return NULL;
}

const char *get_mimetype_for_url(const char *url, char *mimetype) {
    return get_mimetype_for_ext(_get_ext_for_url(url), mimetype);
}

char* _get_ext_for_url(const char *url) {
    char *dot = strrchr(url, '.');
    if(!dot || dot == url) return NULL;
    return dot;
}

// int main() {
//     create_mime_table();
    
//     printf("%s\n", get_mimetype_for_ext(".html", NULL));
//     printf("%s\n", get_mimetype_for_url("/hello.world/index.html", NULL));

//     destroy_mime_table();

//     return 0;
// }