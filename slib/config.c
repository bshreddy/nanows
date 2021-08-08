/**
 * @file slib/config.c
 * @brief A configuration manager.
 *
 * Implements functions defined in `include/config.h`. Used to load, parse and get configurations.
 *
 * Config file is defined by `CONF_FILE` macro in `include/config.h`. Custom config file can be
 * specified by defining `CONF_FILE` macro before `#include "config.h"`.
 *
 * This configuration manager is based on the INI file format and uses *Key-Value file parser*
 * available in glib library. Not all the configurations are implemented yet but the goal is to
 * have most settings configured through the config file defined by `CONF_FILE` macro.
 *
 * Keys for few configuration options are defined in `include/config.h` and can be used as keys
 * to retrive the configuration values. These macros can be changed by defining them before
 * `#include "config.h"`. In which case, the keys for the corresponding configuration options need to
 * be changed accordingly in `CONF_FILE` file.
 *
 * @author Sai Hemanth Bheemreddy (@SaiHemanthBR)
 * @copyright MIT License; Copyright (c) 2021 Sai Hemanth Bheemreddy
 * @bug No known bugs.
 */

#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include "config.h"
#include "helpers.h"

/**
 * @private
 * @brief Key file object to read and store the configuration key-value pairs.
 *
 * This is a private object and should not be accessed directly.
 */
GKeyFile *config = NULL;

/**
 * @private
 * @brief GError object to store the error.
 *
 * This is a private object and should not be accessed directly.
 */
GError *error = NULL;

int load_config() {
    if (config != NULL)
        return 2;

    config = g_key_file_new();
    if (!g_key_file_load_from_file(config, CONF_FILE, 0, &error)) {
        printf("%s\n", error->message);
        free_gerror(&error);
        return 0;
    }

    return 1;
}

char *get_config(const char *key) {
    if (config == NULL)
        return NULL;

    char *value = g_key_file_get_value(config, GROUP_NAME, key, &error);
    if (value == NULL) {
        printf("%s\n", error->message);
        free_gerror(&error);

        return NULL;
    }

    return value;
}

char *get_config_str(const char *key) {
    if (config == NULL)
        return NULL;

    char *value = g_key_file_get_string(config, GROUP_NAME, key, &error);
    if (value == NULL) {
        printf("%s\n", error->message);
        free_gerror(&error);

        return NULL;
    }

    return value;
}

int get_config_int(const char *key) {
    if (config == NULL)
        return INT_MIN;

    if (error != NULL)
        free_gerror(&error);
    int value = g_key_file_get_integer(config, GROUP_NAME, key, &error);
    if (error != NULL) {
        printf("%s\n", error->message);
        free_gerror(&error);
        return INT_MIN;
    }

    return value;
}

void unload_config() {
    if (config == NULL)
        return;

    g_key_file_free(config);
    config = NULL;

    if (error != NULL)
        free_gerror(&error);
}
