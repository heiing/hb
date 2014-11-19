/*
 * File:   httptest.c
 * Author: hzm
 *
 * Created on Nov 10, 2014, 9:32:14 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Basic.h>
#include "../tools.h"
#include "../url.h"

/*
 * CUnit Test Suite
 */

int init_suite(void) {
    return 0;
}

int clean_suite(void) {
    return 0;
}

void test_str_trim() {
    CU_ASSERT_STRING_EQUAL(str_trim(" "), "");
    CU_ASSERT_STRING_EQUAL(str_trim(" hello world!  "), "hello world!");
    CU_ASSERT_STRING_EQUAL(str_ltrim(" hello world!  "), "hello world!  ");
    CU_ASSERT_STRING_EQUAL(str_rtrim(" hello world!  "), " hello world!");
    CU_ASSERT_STRING_EQUAL(str_trim_chars("/-hello world!-/", "/-"), "hello world!");
    CU_ASSERT_STRING_EQUAL(str_ltrim_chars("/-hello world!-/", "/-"), "hello world!-/");
    CU_ASSERT_STRING_EQUAL(str_rtrim_chars("/-hello world!-/", "/-"), "/-hello world!");
}

void url_test(char* url, char* scheme, char* host, int port, char* path, char* query, char* frag, char* user, char* pass) {
    UrlInfo* u = url_parse(url);
    CU_ASSERT_STRING_EQUAL(u->scheme, scheme);
    CU_ASSERT_STRING_EQUAL(u->host, host);
    CU_ASSERT_EQUAL(u->port, port);
    CU_ASSERT_STRING_EQUAL(u->path, path);
    CU_ASSERT_STRING_EQUAL(u->query, query);
    CU_ASSERT_STRING_EQUAL(u->fragment, frag);
    CU_ASSERT_STRING_EQUAL(u->user, user);
    CU_ASSERT_STRING_EQUAL(u->pass, pass);
}

void test_url() {
    url_test("", "", "", 0, "", "", "", "", "");
    url_test("host", "", "host", 0, "", "", "", "", "");
    url_test("#host", "", "", 0, "", "", "host", "", "");
    url_test("?host=@localhost&port=80", "", "", 0, "", "host=@localhost&port=80", "", "", "");
    url_test("/path/to/@file", "", "", 0, "/path/to/@file", "", "", "", "");
    url_test("http://", "http", "", 0, "", "", "", "", "");
    url_test("user@host", "", "host", 0, "", "", "", "user", "");
    url_test("user:123456@host", "", "host", 0, "", "", "", "user", "123456");
    url_test("user:123456@host:/", "", "host", 0, "/", "", "", "user", "123456");
    url_test("https://example.com/api/123?sort=name&by=desc", "https", "example.com", 0, "/api/123", "sort=name&by=desc", "", "", "");
    url_test("http://user:123456@host:89/?host=@localhost&port=80#yes", "http", "host", 89, "/", "host=@localhost&port=80", "yes", "user", "123456");
}

void test_protocal_port() {
    CU_ASSERT_EQUAL(protocal_port("Http"), 80);
    CU_ASSERT_EQUAL(protocal_port("http"), 80);
    CU_ASSERT_EQUAL(protocal_port("HTTP"), 80);
    CU_ASSERT_EQUAL(protocal_port("HTTPs"), 443);
    CU_ASSERT_EQUAL(protocal_port("ftp"), 21);
    CU_ASSERT_EQUAL(protocal_port("ssh"), 22);
    CU_ASSERT_EQUAL(protocal_port("smtp"), 0);
}

int main() {
    CU_pSuite pSuite = NULL;

    /* Initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    /* Add a suite to the registry */
    pSuite = CU_add_suite("httptest", init_suite, clean_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Add the tests to the suite */
    if (
        (NULL == CU_add_test(pSuite, "test_str_trim", test_str_trim)) ||
        (NULL == CU_add_test(pSuite, "test_url", test_url)) ||
        (NULL == CU_add_test(pSuite, "test_protocal_port", test_protocal_port))
        ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
