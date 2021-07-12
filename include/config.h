#ifndef _CONFIG_H
#define _CONFIG_H 1

#ifndef CONF_FILE
    #define CONF_FILE "./etc/nanows.conf"
#endif

#ifndef GROUP_NAME
    #define GROUP_NAME "server"
#endif

#include <glib.h>

bool load_config();
char* get_config(const char*);
char* get_config_str(const char*);
int get_config_int(const char*);

#endif
