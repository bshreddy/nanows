#include <check.h>

#include "mimetypes.h"

START_TEST(test_create_mime_table) {
    // call create_mime_table() and check if it loads the table
    int r_val = create_mime_table();
    ck_assert_int_eq(r_val, 1);

    // call create_mime_table() again and check if it skips loading the table
    int r_val_2 = create_mime_table();
    ck_assert_int_eq(r_val_2, 2);

    destroy_mime_table();
}
END_TEST

START_TEST(test_get_mimetype_for_ext_without_table) {
    // call get_mimetype_for_ext() without calling create_mime_table() and check if it returns NULL
    const char *mimetype = get_mimetype_for_ext(".html", NULL);
    ck_assert_ptr_eq(mimetype, NULL);
}
END_TEST

START_TEST(test_get_mimetype_for_ext) {
    // call create_mime_table() and call get_mimetype_for_ext() and check if it returns the right
    // mimetype
    create_mime_table();
    const char *mimetype_html = get_mimetype_for_ext(".html", NULL);
    ck_assert_str_eq(mimetype_html, "text/html");
}
END_TEST

START_TEST(test_get_mimetype_for_ext_default) {
    // call create_mime_table() and call get_mimetype_for_ext() and check if it returns the right
    // mimetype
    create_mime_table();
    const char *mimetype_xyz = get_mimetype_for_ext(".xyz", NULL);
    ck_assert_str_eq(mimetype_xyz, "application/octet-stream");
}
END_TEST

START_TEST(test_get_mimetype_for_url) {
    // call create_mime_table() and call get_mimetype_for_url() and check if it returns the right
    // mimetype
    create_mime_table();
    const char *mimetype_html = get_mimetype_for_url("http://www.example.com/index.html", NULL);
    ck_assert_str_eq(mimetype_html, "text/html");
}
END_TEST

Suite* mimetypes_suite() {
    const TTest *tests[] = {test_create_mime_table, test_get_mimetype_for_ext_without_table,
                            test_get_mimetype_for_ext, test_get_mimetype_for_ext_default};

    Suite *suite = suite_create("Mimetypes");
    TCase *tc_core = tcase_create("Core");

    for (int t_no = 0; t_no < (sizeof(tests) / sizeof(tests[0])); t_no++)
        tcase_add_test(tc_core, tests[t_no]);
    suite_add_tcase(suite, tc_core);

    return suite;
}

int main() {
    int no_failed;

    Suite *suite = mimetypes_suite();
    SRunner *runner = srunner_create(suite);

    srunner_run_all(runner, CK_NORMAL);
    no_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return (no_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
