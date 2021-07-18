#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "config.h"
#include "helpers.h"

GKeyFile *config = NULL;
GError *error = NULL;

bool load_config() {
    if(config != NULL) return true;

    config = g_key_file_new();
    if(!g_key_file_load_from_file (config, CONF_FILE, 0, &error)) {
        printf("%s\n", error->message);
        free_gerror(&error);
        return false;
    } 

    return true;
}

char* get_config(const char *key) {
    if(config == NULL) return NULL;

    char *value = g_key_file_get_value(config, GROUP_NAME, key, &error);
    if(value == NULL) {
        printf("%s\n", error->message);
        free_gerror(&error);
        
        return NULL;
    }

    return value;
}

char* get_config_str(const char *key) {
    if(config == NULL) return NULL;

    char *value = g_key_file_get_string(config, GROUP_NAME, key, &error);
    if(value == NULL) {
        printf("%s\n", error->message);
        free_gerror(&error);

        return NULL;
    }

    return value;
}

int get_config_int(const char *key) {
    if(config == NULL) return INT_MIN;
    
    if(error != NULL) free_gerror(&error);
    int value = g_key_file_get_integer(config, GROUP_NAME, key, &error);
    if(error != NULL) {
        printf("%s\n", error->message);
        free_gerror(&error);
        return INT_MIN;
    }

    return value;
}

void unload_config() {
    if(config == NULL) return;

    g_key_file_free(config);
    config = NULL;

    if(error != NULL) free_gerror(&error);
}

// int main () {
//     if(!load_config()) return 1;
//     printf("Loaded\n");

//     printf("%s\n", get_config_str("server_host"));
//     printf("%d\n", get_config_int("server_port"));

//     return 0;
// }
