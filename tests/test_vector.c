/* Tests completely by claude while source fully by madhav-fied */


#include <check.h>
#include <clib/vector.h>
#include <stdlib.h>

/* Helper to allocate an int on the heap */
static int *make_int(int val) {
    int *p = malloc(sizeof(int));
    *p = val;
    return p;
}

/* Helper to free all elements stored in a vector */
static void free_elements(vector_t *v) {
    for (size_t i = 0; i < v->size; i++) {
        free(v->data[i]);
    }
}

/* ================================================================== */
/*  1. Lifecycle                                                      */
/* ================================================================== */

START_TEST(test_new_returns_valid_vector) {
    vector_t *v = new_vector(8);
    ck_assert_ptr_nonnull(v);
    ck_assert_uint_eq(v->size, 0);
    ck_assert_uint_ge(v->capacity, 8);
    free_vector(v);
}
END_TEST

START_TEST(test_free_null_is_safe) {
    free_vector(NULL);
}
END_TEST

/* ================================================================== */
/*  2. Empty / size basics                                            */
/* ================================================================== */

START_TEST(test_new_vector_is_empty) {
    vector_t *v = new_vector(4);
    ck_assert(vector_is_empty(v));
    ck_assert_uint_eq(v->size, 0);
    free_vector(v);
}
END_TEST

START_TEST(test_not_empty_after_push) {
    vector_t *v = new_vector(4);
    int *val = make_int(42);
    vector_push_back(v, val);
    ck_assert(!vector_is_empty(v));
    ck_assert_uint_eq(v->size, 1);
    free_elements(v);
    free_vector(v);
}
END_TEST

/* ================================================================== */
/*  3. push_back                                                      */
/* ================================================================== */

START_TEST(test_push_back_single) {
    vector_t *v = new_vector(4);
    int *val = make_int(10);
    vector_push_back(v, val);
    ck_assert_uint_eq(v->size, 1);
    ck_assert_int_eq(*(int *)vector_at(v, 0), 10);
    free_elements(v);
    free_vector(v);
}
END_TEST

START_TEST(test_push_back_multiple) {
    vector_t *v = new_vector(4);
    for (int i = 0; i < 100; i++) {
        vector_push_back(v, make_int(i));
    }
    ck_assert_uint_eq(v->size, 100);
    for (int i = 0; i < 100; i++) {
        ck_assert_int_eq(*(int *)vector_at(v, (size_t)i), i);
    }
    free_elements(v);
    free_vector(v);
}
END_TEST

START_TEST(test_push_back_grows_capacity) {
    vector_t *v = new_vector(4);
    size_t prev_cap = v->capacity;
    for (int i = 0; i < 64; i++) {
        vector_push_back(v, make_int(i));
    }
    ck_assert_uint_ge(v->capacity, 64);
    ck_assert_uint_ge(v->capacity, prev_cap);
    free_elements(v);
    free_vector(v);
}
END_TEST

START_TEST(test_push_back_preserves_data_after_grow) {
    vector_t *v = new_vector(4);
    for (int i = 0; i < 1000; i++) {
        vector_push_back(v, make_int(i));
    }
    for (int i = 0; i < 1000; i++) {
        ck_assert_int_eq(*(int *)vector_at(v, (size_t)i), i);
    }
    free_elements(v);
    free_vector(v);
}
END_TEST

/* ================================================================== */
/*  4. Element access — at                                            */
/* ================================================================== */

START_TEST(test_at_returns_correct_element) {
    vector_t *v = new_vector(8);
    int vals[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) {
        vector_push_back(v, make_int(vals[i]));
    }
    for (int i = 0; i < 5; i++) {
        ck_assert_int_eq(*(int *)vector_at(v, (size_t)i), vals[i]);
    }
    free_elements(v);
    free_vector(v);
}
END_TEST

START_TEST(test_at_out_of_bounds_returns_null) {
    vector_t *v = new_vector(4);
    vector_push_back(v, make_int(1));
    ck_assert_ptr_null(vector_at(v, 1));
    ck_assert_ptr_null(vector_at(v, 100));
    free_elements(v);
    free_vector(v);
}
END_TEST

START_TEST(test_at_on_empty_returns_null) {
    vector_t *v = new_vector(4);
    ck_assert_ptr_null(vector_at(v, 0));
    free_vector(v);
}
END_TEST

/* ================================================================== */
/*  5. pop_back                                                       */
/* ================================================================== */

START_TEST(test_pop_back_decrements_size) {
    vector_t *v = new_vector(4);
    vector_push_back(v, make_int(1));
    vector_push_back(v, make_int(2));
    vector_push_back(v, make_int(3));
    void *popped = vector_pop_back(v);
    ck_assert_ptr_nonnull(popped);
    ck_assert_int_eq(*(int *)popped, 3);
    ck_assert_uint_eq(v->size, 2);
    free(popped);
    free_elements(v);
    free_vector(v);
}
END_TEST

START_TEST(test_pop_back_until_empty) {
    vector_t *v = new_vector(4);
    vector_push_back(v, make_int(10));
    vector_push_back(v, make_int(20));
    vector_push_back(v, make_int(30));
    free(vector_pop_back(v));
    free(vector_pop_back(v));
    free(vector_pop_back(v));
    ck_assert(vector_is_empty(v));
    free_vector(v);
}
END_TEST

START_TEST(test_pop_back_on_empty_returns_null) {
    vector_t *v = new_vector(4);
    ck_assert_ptr_null(vector_pop_back(v));
    free_vector(v);
}
END_TEST

/* ================================================================== */
/*  6. insert                                                         */
/* ================================================================== */

START_TEST(test_insert_at_beginning) {
    vector_t *v = new_vector(4);
    vector_push_back(v, make_int(2));
    vector_push_back(v, make_int(3));
    vector_insert(v, 0, make_int(1));
    ck_assert_uint_eq(v->size, 3);
    ck_assert_int_eq(*(int *)vector_at(v, 0), 1);
    ck_assert_int_eq(*(int *)vector_at(v, 1), 2);
    ck_assert_int_eq(*(int *)vector_at(v, 2), 3);
    free_elements(v);
    free_vector(v);
}
END_TEST

START_TEST(test_insert_at_middle) {
    vector_t *v = new_vector(4);
    vector_push_back(v, make_int(1));
    vector_push_back(v, make_int(3));
    vector_insert(v, 1, make_int(2));
    ck_assert_uint_eq(v->size, 3);
    ck_assert_int_eq(*(int *)vector_at(v, 0), 1);
    ck_assert_int_eq(*(int *)vector_at(v, 1), 2);
    ck_assert_int_eq(*(int *)vector_at(v, 2), 3);
    free_elements(v);
    free_vector(v);
}
END_TEST

START_TEST(test_insert_at_end) {
    vector_t *v = new_vector(4);
    vector_push_back(v, make_int(1));
    vector_push_back(v, make_int(2));
    vector_insert(v, 2, make_int(3));
    ck_assert_uint_eq(v->size, 3);
    ck_assert_int_eq(*(int *)vector_at(v, 2), 3);
    free_elements(v);
    free_vector(v);
}
END_TEST

START_TEST(test_insert_into_empty_vector) {
    vector_t *v = new_vector(4);
    vector_insert(v, 0, make_int(42));
    ck_assert_uint_eq(v->size, 1);
    ck_assert_int_eq(*(int *)vector_at(v, 0), 42);
    free_elements(v);
    free_vector(v);
}
END_TEST

/* ================================================================== */
/*  7. erase                                                          */
/* ================================================================== */

START_TEST(test_erase_first) {
    vector_t *v = new_vector(4);
    vector_push_back(v, make_int(1));
    vector_push_back(v, make_int(2));
    vector_push_back(v, make_int(3));
    free(vector_at(v, 0));
    vector_erase(v, 0);
    ck_assert_uint_eq(v->size, 2);
    ck_assert_int_eq(*(int *)vector_at(v, 0), 2);
    ck_assert_int_eq(*(int *)vector_at(v, 1), 3);
    free_elements(v);
    free_vector(v);
}
END_TEST

START_TEST(test_erase_middle) {
    vector_t *v = new_vector(4);
    vector_push_back(v, make_int(1));
    vector_push_back(v, make_int(2));
    vector_push_back(v, make_int(3));
    free(vector_at(v, 1));
    vector_erase(v, 1);
    ck_assert_uint_eq(v->size, 2);
    ck_assert_int_eq(*(int *)vector_at(v, 0), 1);
    ck_assert_int_eq(*(int *)vector_at(v, 1), 3);
    free_elements(v);
    free_vector(v);
}
END_TEST

START_TEST(test_erase_last) {
    vector_t *v = new_vector(4);
    vector_push_back(v, make_int(1));
    vector_push_back(v, make_int(2));
    vector_push_back(v, make_int(3));
    free(vector_at(v, 2));
    vector_erase(v, 2);
    ck_assert_uint_eq(v->size, 2);
    ck_assert_int_eq(*(int *)vector_at(v, 1), 2);
    free_elements(v);
    free_vector(v);
}
END_TEST

/* ================================================================== */
/*  8. Stress / combined operations                                   */
/* ================================================================== */

START_TEST(test_push_pop_interleaved) {
    vector_t *v = new_vector(4);
    for (int i = 0; i < 50; i++) {
        vector_push_back(v, make_int(i));
    }
    for (int i = 0; i < 25; i++) {
        free(vector_pop_back(v));
    }
    ck_assert_uint_eq(v->size, 25);
    for (int i = 0; i < 25; i++) {
        ck_assert_int_eq(*(int *)vector_at(v, (size_t)i), i);
    }
    for (int i = 100; i < 150; i++) {
        vector_push_back(v, make_int(i));
    }
    ck_assert_uint_eq(v->size, 75);
    ck_assert_int_eq(*(int *)vector_at(v, 25), 100);
    free_elements(v);
    free_vector(v);
}
END_TEST

START_TEST(test_large_vector) {
    vector_t *v = new_vector(16);
    int n = 100000;
    for (int i = 0; i < n; i++) {
        vector_push_back(v, make_int(i));
    }
    ck_assert_uint_eq(v->size, (size_t)n);
    ck_assert_int_eq(*(int *)vector_at(v, 0), 0);
    ck_assert_int_eq(*(int *)vector_at(v, (size_t)(n - 1)), n - 1);
    ck_assert_int_eq(*(int *)vector_at(v, 50000), 50000);
    free_elements(v);
    free_vector(v);
}
END_TEST

/* ================================================================== */
/*  Suite setup                                                       */
/* ================================================================== */

static Suite *vector_suite(void) {
    Suite *s = suite_create("vector");

    /* 1. Lifecycle */
    TCase *tc_lifecycle = tcase_create("lifecycle");
    tcase_add_test(tc_lifecycle, test_new_returns_valid_vector);
    tcase_add_test(tc_lifecycle, test_free_null_is_safe);
    suite_add_tcase(s, tc_lifecycle);

    /* 2. Empty / size */
    TCase *tc_capacity = tcase_create("capacity");
    tcase_add_test(tc_capacity, test_new_vector_is_empty);
    tcase_add_test(tc_capacity, test_not_empty_after_push);
    suite_add_tcase(s, tc_capacity);

    /* 3. push_back */
    TCase *tc_push = tcase_create("push_back");
    tcase_add_test(tc_push, test_push_back_single);
    tcase_add_test(tc_push, test_push_back_multiple);
    tcase_add_test(tc_push, test_push_back_grows_capacity);
    tcase_add_test(tc_push, test_push_back_preserves_data_after_grow);
    suite_add_tcase(s, tc_push);

    /* 4. Element access */
    TCase *tc_access = tcase_create("element_access");
    tcase_add_test(tc_access, test_at_returns_correct_element);
    tcase_add_test(tc_access, test_at_out_of_bounds_returns_null);
    tcase_add_test(tc_access, test_at_on_empty_returns_null);
    suite_add_tcase(s, tc_access);

    /* 5. pop_back */
    TCase *tc_pop = tcase_create("pop_back");
    tcase_add_test(tc_pop, test_pop_back_decrements_size);
    tcase_add_test(tc_pop, test_pop_back_until_empty);
    tcase_add_test(tc_pop, test_pop_back_on_empty_returns_null);
    suite_add_tcase(s, tc_pop);

    /* 6. insert */
    TCase *tc_insert = tcase_create("insert");
    tcase_add_test(tc_insert, test_insert_at_beginning);
    tcase_add_test(tc_insert, test_insert_at_middle);
    tcase_add_test(tc_insert, test_insert_at_end);
    tcase_add_test(tc_insert, test_insert_into_empty_vector);
    suite_add_tcase(s, tc_insert);

    /* 7. erase */
    TCase *tc_erase = tcase_create("erase");
    tcase_add_test(tc_erase, test_erase_first);
    tcase_add_test(tc_erase, test_erase_middle);
    tcase_add_test(tc_erase, test_erase_last);
    suite_add_tcase(s, tc_erase);

    /* 8. Stress */
    TCase *tc_stress = tcase_create("stress");
    tcase_set_timeout(tc_stress, 10);
    tcase_add_test(tc_stress, test_push_pop_interleaved);
    tcase_add_test(tc_stress, test_large_vector);
    suite_add_tcase(s, tc_stress);

    return s;
}

int main(void) {
    Suite *s = vector_suite();
    SRunner *sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    int nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
