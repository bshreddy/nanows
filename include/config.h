/**
 * @file include/config.h
 * @brief Function Prototypes for the configuration manager.
 *
 * This file contains function prototypes for loading, retriving configurations from file defined by
 * `CONF_FILE` macro.
 *
 * Implemented in `slib/config.c`.
 *
 * @author Sai Hemanth Bheemreddy (@SaiHemanthBR)
 * @copyright MIT License; Copyright (c) 2021 Sai Hemanth Bheemreddy
 * @bug No known bugs.
 */

#ifndef _CONFIG_H
#define _CONFIG_H 1

#ifndef CONF_FILE
#define CONF_FILE "./etc/nanows.conf"
#endif

#ifndef GROUP_NAME
#define GROUP_NAME "server"
#endif

#ifndef HOST_CONF_KEY
#define HOST_CONF_KEY "server_host"
#endif

#ifndef PORT_CONF_KEY
#define PORT_CONF_KEY "server_port"
#endif

#ifndef SITE_DIR_CONF_KEY
#define SITE_DIR_CONF_KEY "site_root_dir"
#endif

#ifndef PAGE_CONF_KEY
#define PAGE_CONF_KEY "default_page"
#endif

#include <glib.h>

int load_config();
char *get_config(const char *);
char *get_config_str(const char *);
int get_config_int(const char *);
void unload_config();

#endif
