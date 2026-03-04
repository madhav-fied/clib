/* Tests completely by claude while source fully by madhav-fied */

#include <check.h>
#include <clib/stack.h>
#include <stdlib.h>

static int *make_int(int val) {
    int *p = malloc(sizeof(int));
    *p = val;
    return p;
}

static void free_elements(stack_t *s) {
    for (size_t i = 0; i < s->size; i++) {
        free(s->data[i]);
    }
}

/* ================================================================== */
/*  1. Lifecycle                                                       */
/* ================================================================== */

START_TEST(test_new_returns_valid_stack) {
    stack_t *s = new_stack(8);
    ck_assert_ptr_nonnull(s);
    ck_assert_uint_eq(s->size, 0);
    ck_assert_uint_ge(s->capacity, 8);
    free_stack(s);
}
END_TEST

START_TEST(test_free_null_is_safe) {
    free_stack(NULL);
}
END_TEST

/* ================================================================== */
/*  2. Empty / size basics                                             */
/* ================================================================== */

START_TEST(test_new_stack_is_empty) {
    stack_t *s = new_stack(4);
    ck_assert(stack_is_empty(s));
    ck_assert_uint_eq(s->size, 0);
    free_stack(s);
}
END_TEST

START_TEST(test_not_empty_after_push) {
    stack_t *s = new_stack(4);
    int *val = make_int(42);
    stack_push(s, val);
    ck_assert(!stack_is_empty(s));
    ck_assert_uint_eq(s->size, 1);
    free_elements(s);
    free_stack(s);
}
END_TEST

/* ================================================================== */
/*  3. push                                                            */
/* ================================================================== */

START_TEST(test_push_single) {
    stack_t *s = new_stack(4);
    int *val = make_int(10);
    stack_push(s, val);
    ck_assert_uint_eq(s->size, 1);
    ck_assert_int_eq(*(int *)stack_top(s), 10);
    free_elements(s);
    free_stack(s);
}
END_TEST

START_TEST(test_push_multiple_lifo_order) {
    stack_t *s = new_stack(4);
    stack_push(s, make_int(1));
    stack_push(s, make_int(2));
    stack_push(s, make_int(3));
    ck_assert_uint_eq(s->size, 3);
    /* top should be the last pushed element */
    ck_assert_int_eq(*(int *)stack_top(s), 3);
    free_elements(s);
    free_stack(s);
}
END_TEST

START_TEST(test_push_grows_capacity) {
    stack_t *s = new_stack(4);
    for (int i = 0; i < 64; i++) {
        stack_push(s, make_int(i));
    }
    ck_assert_uint_eq(s->size, 64);
    ck_assert_uint_ge(s->capacity, 64);
    free_elements(s);
    free_stack(s);
}
END_TEST

/* ================================================================== */
/*  4. top                                                             */
/* ================================================================== */

START_TEST(test_top_returns_last_pushed) {
    stack_t *s = new_stack(4);
    stack_push(s, make_int(100));
    stack_push(s, make_int(200));
    ck_assert_int_eq(*(int *)stack_top(s), 200);
    /* top must not remove the element */
    ck_assert_uint_eq(s->size, 2);
    free_elements(s);
    free_stack(s);
}
END_TEST

START_TEST(test_top_on_empty_returns_null) {
    stack_t *s = new_stack(4);
    ck_assert_ptr_null(stack_top(s));
    free_stack(s);
}
END_TEST

START_TEST(test_top_does_not_remove_element) {
    stack_t *s = new_stack(4);
    stack_push(s, make_int(7));
    stack_top(s);
    stack_top(s);
    ck_assert_uint_eq(s->size, 1);
    free_elements(s);
    free_stack(s);
}
END_TEST

/* ================================================================== */
/*  5. pop                                                             */
/* ================================================================== */

START_TEST(test_pop_returns_top_and_removes) {
    stack_t *s = new_stack(4);
    stack_push(s, make_int(1));
    stack_push(s, make_int(2));
    stack_push(s, make_int(3));
    void *val = stack_pop(s);
    ck_assert_ptr_nonnull(val);
    ck_assert_int_eq(*(int *)val, 3);
    ck_assert_uint_eq(s->size, 2);
    free(val);
    free_elements(s);
    free_stack(s);
}
END_TEST

START_TEST(test_pop_lifo_order) {
    stack_t *s = new_stack(4);
    stack_push(s, make_int(10));
    stack_push(s, make_int(20));
    stack_push(s, make_int(30));

    void *a = stack_pop(s);
    void *b = stack_pop(s);
    void *c = stack_pop(s);

    ck_assert_int_eq(*(int *)a, 30);
    ck_assert_int_eq(*(int *)b, 20);
    ck_assert_int_eq(*(int *)c, 10);

    free(a); free(b); free(c);
    free_stack(s);
}
END_TEST

START_TEST(test_pop_until_empty) {
    stack_t *s = new_stack(4);
    stack_push(s, make_int(1));
    stack_push(s, make_int(2));
    free(stack_pop(s));
    free(stack_pop(s));
    ck_assert(stack_is_empty(s));
    free_stack(s);
}
END_TEST

START_TEST(test_pop_on_empty_returns_null) {
    stack_t *s = new_stack(4);
    ck_assert_ptr_null(stack_pop(s));
    free_stack(s);
}
END_TEST

/* ================================================================== */
/*  6. Stress / combined operations                                    */
/* ================================================================== */

START_TEST(test_push_pop_interleaved) {
    stack_t *s = new_stack(4);
    for (int i = 0; i < 50; i++) {
        stack_push(s, make_int(i));
    }
    for (int i = 49; i >= 25; i--) {
        void *val = stack_pop(s);
        ck_assert_int_eq(*(int *)val, i);
        free(val);
    }
    ck_assert_uint_eq(s->size, 25);
    for (int i = 100; i < 125; i++) {
        stack_push(s, make_int(i));
    }
    ck_assert_uint_eq(s->size, 50);
    /* top should now be 124 */
    ck_assert_int_eq(*(int *)stack_top(s), 124);
    free_elements(s);
    free_stack(s);
}
END_TEST

START_TEST(test_large_stack) {
    stack_t *s = new_stack(16);
    int n = 100000;
    for (int i = 0; i < n; i++) {
        stack_push(s, make_int(i));
    }
    ck_assert_uint_eq(s->size, (size_t)n);
    ck_assert_int_eq(*(int *)stack_top(s), n - 1);
    free_elements(s);
    free_stack(s);
}
END_TEST

/* ================================================================== */
/*  Suite setup                                                        */
/* ================================================================== */

static Suite *stack_suite(void) {
    Suite *s = suite_create("stack");

    /* 1. Lifecycle */
    TCase *tc_lifecycle = tcase_create("lifecycle");
    tcase_add_test(tc_lifecycle, test_new_returns_valid_stack);
    tcase_add_test(tc_lifecycle, test_free_null_is_safe);
    suite_add_tcase(s, tc_lifecycle);

    /* 2. Empty / size */
    TCase *tc_empty = tcase_create("empty");
    tcase_add_test(tc_empty, test_new_stack_is_empty);
    tcase_add_test(tc_empty, test_not_empty_after_push);
    suite_add_tcase(s, tc_empty);

    /* 3. push */
    TCase *tc_push = tcase_create("push");
    tcase_add_test(tc_push, test_push_single);
    tcase_add_test(tc_push, test_push_multiple_lifo_order);
    tcase_add_test(tc_push, test_push_grows_capacity);
    suite_add_tcase(s, tc_push);

    /* 4. top */
    TCase *tc_top = tcase_create("top");
    tcase_add_test(tc_top, test_top_returns_last_pushed);
    tcase_add_test(tc_top, test_top_on_empty_returns_null);
    tcase_add_test(tc_top, test_top_does_not_remove_element);
    suite_add_tcase(s, tc_top);

    /* 5. pop */
    TCase *tc_pop = tcase_create("pop");
    tcase_add_test(tc_pop, test_pop_returns_top_and_removes);
    tcase_add_test(tc_pop, test_pop_lifo_order);
    tcase_add_test(tc_pop, test_pop_until_empty);
    tcase_add_test(tc_pop, test_pop_on_empty_returns_null);
    suite_add_tcase(s, tc_pop);

    /* 6. Stress */
    TCase *tc_stress = tcase_create("stress");
    tcase_set_timeout(tc_stress, 10);
    tcase_add_test(tc_stress, test_push_pop_interleaved);
    tcase_add_test(tc_stress, test_large_stack);
    suite_add_tcase(s, tc_stress);

    return s;
}

int main(void) {
    Suite *s = stack_suite();
    SRunner *sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    int nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
