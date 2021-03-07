#ifndef _MIMETYPES_H
#define _MIMETYPES_H 1

#include<stdlib.h>
#include<fcntl.h>
#include<errno.h>
#include<string.h>

#define __USE_GNU 1
#include<search.h>

#ifndef MIME_CONF_FILE
    #define MIME_CONF_FILE "mimetypes.conf"
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

struct hsearch_data *_mime_htab = NULL;

int create_mime_table();
void destroy_mime_table();
char* get_mimetype_for_ext(char*, char*);
char* get_mimetype_for_url(char*, char*);

char* _get_ext_for_path(char*);

int create_mime_table() {
    if(_mime_htab != NULL) return 1;

    FILE *mime_file = NULL;
    ENTRY entry, *ent;
    _mime_htab = malloc(sizeof(struct hsearch_data));

    char buf[MIME_BUF_SIZE];

    if(!hcreate_r(MIME_HTABLE_SIZE, _mime_htab)) {
        printf("Can't create hash table: %s\n", strerror(errno));
        return 0;
    }

    if((mime_file = fopen(MIME_CONF_FILE, "r")) == NULL) {
        printf("Can't open file (%s): %s\n", MIME_CONF_FILE, strerror(errno));
        return 0;
    }
    
    while(fgets(buf, MIME_BUF_SIZE, mime_file) != NULL) {
        char *_key = strtok(buf, "=");
        char *_val = strtok(NULL, "=\n");
        char *key = malloc(strlen(_key));
        char *val = malloc(strlen(_val));
        strcpy(key, _key);
        strcpy(val, _val);

        entry.key = key;
        entry.data = (void*) val;

        if(!hsearch_r(entry, ENTER, &ent, _mime_htab)) {
            printf("Can't insert entry into hash table: %s\n", strerror(errno));
            return 0;
        }
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

char* get_mimetype_for_ext(char *ext, char* mimetype) {
    if(_mime_htab == NULL) return NULL;
    ENTRY entry, *ent;

    if(ext != NULL){
        entry.key = ext;
        if(hsearch_r(entry, FIND, &ent, _mime_htab)) {
            if(mimetype != NULL) strcpy(mimetype, (char*)(ent->data));
            return (char*)(ent->data);
        }
    }

    entry.key = DEFAULT_MIMETYPE_KEY;
    if(hsearch_r(entry, FIND, &ent, _mime_htab)) {
        if(mimetype != NULL) strcpy(mimetype, (char*)(ent->data));
        return (char*)(ent->data);
    }

    return NULL;
}

char* get_mimetype_for_url(char *url, char* mimetype) {
    return get_mimetype_for_ext(_get_ext_for_path(url), mimetype);
}

char* _get_ext_for_path(char *pathname) {
    char *dot = strrchr(pathname, '.');
    if(!dot || dot == pathname) return NULL;
    return dot;
}

#endif