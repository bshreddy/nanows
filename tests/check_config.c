#include <check.h>

#include "config.h"

START_TEST(test_check_config) {
    // call load_config() and check if it loads the config
    int r_val = load_config();
    ck_assert_int_eq(r_val, 1);

    // call load_config() again and check if it skips loading
    int r_val_2 = load_config();
    ck_assert_int_eq(r_val_2, 2);

    unload_config();
}
END_TEST

START_TEST(test_get_config_without_load) {
    // call get_config() without loading config and check if it returns NULL
    char *val = get_config("server_host");
    ck_assert_ptr_eq(val, NULL);
}
END_TEST

START_TEST(test_get_config_str_without_load) {
    // call get_config_str() without loading config and check if it returns NULL
    char *val = get_config_str("server_host");
    ck_assert_ptr_eq(val, NULL);
}
END_TEST

START_TEST(test_get_config_int_without_load) {
    // call get_config_int() without loading config and check if it returns INT_MIN
    int val = get_config_int("server_port");
    ck_assert_int_eq(val, INT_MIN);
}
END_TEST

START_TEST(test_get_config_valid_key) {
    // call load_config() and call get_config() with valid key and check if it returns correct value
    load_config();
    char *val = get_config("server_host");
    ck_assert_str_eq(val, "127.0.0.1");

    unload_config();
}
END_TEST

START_TEST(test_get_config_invalid_key) {
    // call load_config() and call get_config() with invalid key and check if it returns NULL
    load_config();
    char *val = get_config("invalid_key");
    ck_assert_ptr_eq(val, NULL);

    unload_config();
}

START_TEST(test_get_config_str_valid_key) {
    // call load_config() and call get_config_str() with valid key and check if it returns correct
    // value
    load_config();
    char *val = get_config_str("server_host");
    ck_assert_str_eq(val, "127.0.0.1");

    unload_config();
}
END_TEST

START_TEST(test_get_config_str_invalid_key) {
    // call load_config() and call get_config_str() with invalid key and check if it returns NULL
    load_config();
    char *val = get_config_str("invalid_key");
    ck_assert_ptr_eq(val, NULL);

    unload_config();
}
END_TEST

START_TEST(test_get_config_int_valid_key) {
    // call load_config() and call get_config_int() with valid key and check if it returns correct
    // value
    load_config();
    int val = get_config_int("server_port");
    ck_assert_int_eq(val, 8080);

    unload_config();
}
END_TEST

START_TEST(test_get_config_int_invalid_key) {
    // call load_config() and call get_config_int() with invalid key and check if it returns INT_MIN
    load_config();
    int val = get_config_int("invalid_key");
    ck_assert_int_eq(val, INT_MIN);

    unload_config();
}
END_TEST

Suite *config_suite() {
    const TTest *tests[] = {test_check_config,
                            test_get_config_without_load,
                            test_get_config_str_without_load,
                            test_get_config_int_without_load,
                            test_get_config_valid_key,
                            test_get_config_invalid_key,
                            test_get_config_str_valid_key,
                            test_get_config_str_invalid_key,
                            test_get_config_int_valid_key,
                            test_get_config_int_invalid_key};

    Suite *suite = suite_create("Config");
    TCase *tc_core = tcase_create("Core");

    for (int t_no = 0; t_no < sizeof(tests) / sizeof(tests[0]); t_no++)
        tcase_add_test(tc_core, tests[t_no]);
    suite_add_tcase(suite, tc_core);

    return suite;
}

int main() {
    int no_failed;

    Suite *suite = config_suite();
    SRunner *runner = srunner_create(suite);

    srunner_run_all(runner, CK_NORMAL);
    no_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return (no_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
