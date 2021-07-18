#include <check.h>
#include <stdio.h>

#include "request.h"
#include "response.h"

START_TEST(test__initialize_response) {
    // call _initialize_response() and check if the response is initialized with default values.
    response *res = _initialize_response();

    ck_assert_ptr_ne(res, NULL);
    ck_assert_int_eq(res->conn_fd, -1);
    ck_assert_ptr_eq(res->http_ver, NULL);
    ck_assert_ptr_eq(res->status_code, NULL);
    ck_assert_int_eq(g_hash_table_size(res->header_htab), 0);

    _free_response(res);
}
END_TEST

START_TEST(test__free_response) {
    // call _initialize_response() to initialize a response with default values.
    response *res = _initialize_response();

    // call _free_response() and check if the response is freed.
    _free_response(res);
    ck_assert_ptr_eq(res, NULL);
}
END_TEST

START_TEST(test_create_response_from_request) {
    char req_buf[] = "GET / HTTP/1.1\r\n\r\n";

    // Create a sample request to test create_response_from_request() function.
    request *req = _initialize_request();
    req->conn_fd = 1;
    int ret_val = _parse_request(req_buf, req);
    ck_assert_int_eq(ret_val, 1);

    // call create_response_from_request() and check if the response is created from request.
    response *res = create_response_from_request(req);

    ck_assert_ptr_ne(res, NULL);
    ck_assert_int_ne(res->conn_fd, -1);
    ck_assert_int_ne(res->conn_fd, req->conn_fd);
    ck_assert_str_eq(res->http_ver, req->http_ver);
    ck_assert_ptr_eq(res->status_code, NULL);
    ck_assert_int_eq(g_hash_table_size(res->header_htab), 0);

    _free_request(req);
    close_response(res);
}

START_TEST(test_create_response_from_default_request) {
    // Create a default request to test create_response_from_request() function.
    request *req = _initialize_request();
    ck_assert_ptr_ne(req, NULL);

    // call create_response_from_request() and check if the response is created from request.
    response *res = create_response_from_request(req);

    ck_assert_ptr_ne(res, NULL);
    ck_assert_int_eq(res->conn_fd, -1);
    ck_assert_ptr_eq(res->http_ver, NULL);
    ck_assert_ptr_eq(res->status_code, NULL);
    ck_assert_int_eq(g_hash_table_size(res->header_htab), 0);

    _free_request(req);
    _free_response(res);
}

START_TEST(test_create_response_from_null_request) {
    // call create_response_from_request() with null request and check if the response is created
    // from request.
    response *res = create_response_from_request(NULL);

    ck_assert_ptr_ne(res, NULL);
}

START_TEST(test_set_response_header) {
    // call _initialize_response() to initialize a response with default values.
    response *res = _initialize_response();
    ck_assert_ptr_ne(res, NULL);

    // call set_response_header() with 3 headers and check if the count is 3.
    set_response_header(res, "Content-Type", "text/html");
    set_response_header(res, "Content-Length", "100");
    set_response_header(res, "Connection", "close");
    ck_assert_int_eq(g_hash_table_size(res->header_htab), 3);

    _free_response(res);
}
END_TEST

START_TEST(test_get_response_header) {
    // call _initialize_response() to initialize a response with default values.
    response *res = _initialize_response();
    ck_assert_ptr_ne(res, NULL);

    // call set_response_header() with 2 headers.
    set_response_header(res, "Content-Type", "text/html");
    set_response_header(res, "Content-Length", "100");

    // call get_response_header() for 2 headers and check if the header fields are returned.
    const char *header_field = get_response_header(res, "Content-Type", NULL);
    ck_assert_str_eq(header_field, "text/html");
    header_field = get_response_header(res, "Content-Length", NULL);
    ck_assert_str_eq(header_field, "100");

    // call get_response_header() for a non-existing header and check if NULL is returned.
    header_field = get_response_header(res, "Connection", NULL);
    ck_assert_ptr_eq(header_field, NULL);

    _free_response(res);
}
END_TEST

Suite *response_suite() {
    const TTest *tests[] = {test__initialize_response,
                            test__free_response,
                            test_create_response_from_request,
                            test_create_response_from_default_request,
                            test_create_response_from_null_request,
                            test_set_response_header,
                            test_get_response_header};

    Suite *suite = suite_create("Response");
    TCase *tc_core = tcase_create("Core");

    for (int t_no = 0; t_no < sizeof(tests) / sizeof(tests[0]); t_no++)
        tcase_add_test(tc_core, tests[t_no]);
    suite_add_tcase(suite, tc_core);
    return suite;
}

int main() {
    int no_failed;

    Suite *suite = response_suite();
    SRunner *runner = srunner_create(suite);

    srunner_run_all(runner, CK_NORMAL);
    no_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return (no_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
