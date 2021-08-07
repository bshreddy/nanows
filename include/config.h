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

/**
 * @brief Defines the default configuration file in INI file format.
 */
#ifndef CONF_FILE
#define CONF_FILE "./etc/nanows.conf"
#endif

/**
 * @brief Defines the default group name for server configuration options.
 */
#ifndef GROUP_NAME
#define GROUP_NAME "server"
#endif

/**
 * @brief Defines the default configuration key for Server IP.
 */
#ifndef HOST_CONF_KEY
#define HOST_CONF_KEY "server_host"
#endif

/**
 * @brief Defines the default configuration key for Server Port.
 */
#ifndef PORT_CONF_KEY
#define PORT_CONF_KEY "server_port"
#endif

/**
 * @brief Defines the default configuration key for website root directory.
 */
#ifndef SITE_DIR_CONF_KEY
#define SITE_DIR_CONF_KEY "site_root_dir"
#endif

/**
 * @brief Defines the default configuration key for website default file.
 */
#ifndef PAGE_CONF_KEY
#define PAGE_CONF_KEY "default_page"
#endif

#include <glib.h>

/**
 * @brief Loads and parses the configuration file and returns an int value to indicate success or
 * failure.
 *
 * Parsing the configuration file is done using the `g_key_file_load_from_file()` function. The
 * config file is loaded from the `CONF_FILE` macro. Cofiguration file is expected to be in INI
 * format.
 *
 * If the configuration file is loaded successfully, the function returns `1` and the parsed config
 * is stored in the `config` variable. If the configuration file is already loaded (i.e. `config` is
 * not `NULL`), the function returns `2` without performing any action. If the configuration file is
 * not loaded successfully, the function returns `0`.
 *
 * @return On success, returns a non-zero value. On failure, returns 0.
 */
int load_config();

/**
 * @brief Returns the value for the corresponding configuration key.
 *
 * Key is retrived using the `g_key_file_get_value()` function. The key is expected to valid key
 * defined in configuration file. Default keys have macros defined in `include/config.h` like
 * `HOST_CONF_KEY`, `PORT_CONF_KEY`, etc.
 *
 * @param key The configuration key.
 * @return a newly allocated string or `NULL`, if the specified key cannot be found.
 */
char *get_config(const char *);

/**
 * @brief Returns the value for the corresponding configuration key.
 *
 * Key is retrived using the `g_key_file_get_string()` function. This function is similar to
 * `get_config()` except that it handles escape sequences like `\s`.
 *
 * @param key The configuration key.
 * @return a newly allocated string or `NULL`, if the specified key cannot be found.
 */
char *get_config_str(const char *);

/**
 * @brief Returns the int value for the corresponding configuration key.
 *
 * Key is retrived using the `g_key_file_get_integer()` function. This function is similar to
 * `get_config()` except it returns an integer value.
 *
 * @param key The configuration key.
 * @return The value associated with the key as an integer, or `INT_MIN`, if the key was not found.
 */
int get_config_int(const char *);

/**
 * @brief Unloads configuration and frees memory allocated for configuration and any errors.
 *
 * @return void
 */
void unload_config();

#endif
