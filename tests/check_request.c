#include <check.h>

#include "request.h"

char req_buf[REQ_BUF_SIZE] =
    "GET / HTTP/1.1\r\n"
    "Host: localhost:8080\r\n"
    "Connection: keep-alive\r\n"
    "Cache-Control: max-age=0\r\n"
    "Upgrade-Insecure-Requests: 1\r\n"
    "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 11_2_0) AppleWebKit/537.36 (KHTML, like "
    "Gecko) Chrome/87.0.4280.88 Safari/537.36\r\n"
    "Accept: "
    "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/"
    "*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n"
    "Sec-Fetch-Site: none\r\n"
    "Sec-Fetch-Mode: navigate\r\n"
    "Sec-Fetch-User: ?1\r\n"
    "Sec-Fetch-Dest: document\r\n"
    "Accept-Encoding: gzip, deflate, br\r\n"
    "Accept-Language: en-US,en;q=0.9\r\n"
    "\r\n";

START_TEST(test__initialize_request) {
    // Call _initialize_request() and check if the request is initialized with default values.
    request *req = _initialize_request();

    ck_assert_ptr_ne(req, NULL);
    ck_assert_int_eq(req->conn_fd, -1);
    ck_assert_ptr_eq(req->http_method, NULL);
    ck_assert_ptr_eq(req->url, NULL);
    ck_assert_ptr_eq(req->http_ver, NULL);
    ck_assert_int_eq(g_hash_table_size(req->header_htab), 0);

    _free_request(req);
}
END_TEST

START_TEST(test__parse_request) {
    // Call _parse_request() and check if the request is parsed correctly.
    request *req = _initialize_request();
    int ret_val = _parse_request(req_buf, req);
    ck_assert_int_eq(ret_val, 1);

    ck_assert_int_eq(req->conn_fd, -1);
    ck_assert_str_eq(req->http_method, "GET");
    ck_assert_str_eq(req->url, "/");
    ck_assert_str_eq(req->http_ver, "HTTP/1.1");
    ck_assert_int_eq(g_hash_table_size(req->header_htab), 12);

    _free_request(req);
}
END_TEST

START_TEST(test__parse_request_null_req_buf) {
    // Call _parse_request() with NULL request buffer and check if it returns NULL.
    request *req = _initialize_request();
    int ret_val = _parse_request(NULL, req);
    ck_assert_int_eq(ret_val, 0);

    _free_request(req);
}
END_TEST

START_TEST(test__parse_request_null_req) {
    // Call _parse_request() with NULL request and check if it returns 0.
    int ret_val = _parse_request(req_buf, NULL);
    ck_assert_int_eq(ret_val, 0);
}
END_TEST

START_TEST(test__free_request) {
    // Call _initialize_request() to initialize a request with default values.
    request *req = _initialize_request();

    // Call _free_request() and check if the request is freed and set to NULL.
    _free_request(req);
    ck_assert_ptr_eq(req, NULL);
}
END_TEST

START_TEST(test_get_request_header) {
    // Create a sample request to test get_request_header() function.
    request *req = _initialize_request();
    int ret_val = _parse_request(req_buf, req);
    ck_assert_int_eq(ret_val, 1);

    // Check if all the request header fields are parsed correctly.
    ck_assert_int_eq(g_hash_table_size(req->header_htab), 12);

    // Check if the request header field Host is parsed correctly.
    const char *val = get_request_header(req, "Host", NULL);
    ck_assert_str_eq(val, "localhost:8080");

    // Check if the request header field Accept is parsed correctly.
    val = get_request_header(req, "Accept", NULL);
    ck_assert_str_eq(val, "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/"
                          "webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9");

    // Check if the request header field User-Agent is parsed correctly.
    val = get_request_header(req, "User-Agent", NULL);
    ck_assert_str_eq(val,
                     "Mozilla/5.0 (Macintosh; Intel Mac OS X 11_2_0) AppleWebKit/537.36 (KHTML, "
                     "like Gecko) Chrome/87.0.4280.88 Safari/537.36");
    _free_request(req);
}
END_TEST

START_TEST(test_get_request_header_undefined_field) {
    // Create a sample request to test get_request_header() function.
    request *req = _initialize_request();
    int ret_val = _parse_request(req_buf, req);
    ck_assert_int_eq(ret_val, 1);

    // Check if the get_request_header returns NULL when the header field is not found or
    // not-defined.
    const char *val = get_request_header(req, "Not-Defined", NULL);
    ck_assert_ptr_eq(val, NULL);
    _free_request(req);
}
END_TEST

START_TEST(test_get_request_header_null_field) {
    // Create a sample request to test get_request_header() function.
    request *req = _initialize_request();
    int ret_val = _parse_request(req_buf, req);
    ck_assert_int_eq(ret_val, 1);

    // Check if the get_request_header returns NULL when the header field is NULL.
    const char *val = get_request_header(req, NULL, NULL);
    ck_assert_ptr_eq(val, NULL);
    _free_request(req);
}
END_TEST

START_TEST(test_get_request_header_null_req) {
    // Create a sample request to test get_request_header() function.
    request *req = _initialize_request();
    int ret_val = _parse_request(req_buf, req);
    ck_assert_int_eq(ret_val, 1);

    // Check if the get_request_header returns NULL when the request is NULL.
    const  char *val = get_request_header(NULL, "Host", NULL);
    ck_assert_ptr_eq(val, NULL);
    _free_request(req);
}
END_TEST

Suite *request_suite() {
    const TTest *tests[] = {test__initialize_request,
                            test__parse_request,
                            test__parse_request_null_req_buf,
                            test__parse_request_null_req,
                            test__free_request,
                            test_get_request_header,
                            test_get_request_header_undefined_field,
                            test_get_request_header_null_field,
                            test_get_request_header_null_req};

    Suite *suite = suite_create("Request");
    TCase *tc_core = tcase_create("Core");

    for (int t_no = 0; t_no < sizeof(tests) / sizeof(tests[0]); t_no++)
        tcase_add_test(tc_core, tests[t_no]);
    suite_add_tcase(suite, tc_core);

    return suite;
}

int main() {
    int no_failed;

    Suite *suite = request_suite();
    SRunner *runner = srunner_create(suite);

    srunner_run_all(runner, CK_NORMAL);
    no_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return (no_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
