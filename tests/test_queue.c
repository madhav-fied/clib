/* Tests completely by claude while source fully by madhav-fied */

#include <check.h>
#include <clib/queue.h>
#include <stdlib.h>

static int *make_int(int val) {
    int *p = malloc(sizeof(int));
    *p = val;
    return p;
}

static void free_elements(queue_t *q) {
    for (size_t i = 0; i < q->size; i++) {
        free(q->data[i]);
    }
}

/* ================================================================== */
/*  1. Lifecycle                                                       */
/* ================================================================== */

START_TEST(test_new_returns_valid_queue) {
    queue_t *q = new_queue(8);
    ck_assert_ptr_nonnull(q);
    ck_assert_uint_eq(q->size, 0);
    ck_assert_uint_ge(q->capacity, 8);
    free_queue(q);
}
END_TEST

START_TEST(test_free_null_is_safe) {
    free_queue(NULL);
}
END_TEST

/* ================================================================== */
/*  2. Empty / size basics                                             */
/* ================================================================== */

START_TEST(test_new_queue_is_empty) {
    queue_t *q = new_queue(4);
    ck_assert(queue_is_empty(q));
    ck_assert_uint_eq(q->size, 0);
    free_queue(q);
}
END_TEST

START_TEST(test_not_empty_after_push) {
    queue_t *q = new_queue(4);
    int *val = make_int(42);
    queue_push(q, val);
    ck_assert(!queue_is_empty(q));
    ck_assert_uint_eq(q->size, 1);
    free_elements(q);
    free_queue(q);
}
END_TEST

/* ================================================================== */
/*  3. push                                                            */
/* ================================================================== */

START_TEST(test_push_single) {
    queue_t *q = new_queue(4);
    int *val = make_int(10);
    queue_push(q, val);
    ck_assert_uint_eq(q->size, 1);
    ck_assert_int_eq(*(int *)queue_front(q), 10);
    free_elements(q);
    free_queue(q);
}
END_TEST

START_TEST(test_push_multiple_fifo_order) {
    queue_t *q = new_queue(4);
    queue_push(q, make_int(1));
    queue_push(q, make_int(2));
    queue_push(q, make_int(3));
    ck_assert_uint_eq(q->size, 3);
    /* front should be the first pushed element */
    ck_assert_int_eq(*(int *)queue_front(q), 1);
    /* back should be the last pushed element */
    ck_assert_int_eq(*(int *)queue_back(q), 3);
    free_elements(q);
    free_queue(q);
}
END_TEST

START_TEST(test_push_grows_capacity) {
    queue_t *q = new_queue(4);
    for (int i = 0; i < 64; i++) {
        queue_push(q, make_int(i));
    }
    ck_assert_uint_eq(q->size, 64);
    ck_assert_uint_ge(q->capacity, 64);
    free_elements(q);
    free_queue(q);
}
END_TEST

/* ================================================================== */
/*  4. front / back                                                    */
/* ================================================================== */

START_TEST(test_front_returns_oldest_element) {
    queue_t *q = new_queue(4);
    queue_push(q, make_int(100));
    queue_push(q, make_int(200));
    ck_assert_int_eq(*(int *)queue_front(q), 100);
    /* front must not remove the element */
    ck_assert_uint_eq(q->size, 2);
    free_elements(q);
    free_queue(q);
}
END_TEST

START_TEST(test_back_returns_newest_element) {
    queue_t *q = new_queue(4);
    queue_push(q, make_int(100));
    queue_push(q, make_int(200));
    ck_assert_int_eq(*(int *)queue_back(q), 200);
    /* back must not remove the element */
    ck_assert_uint_eq(q->size, 2);
    free_elements(q);
    free_queue(q);
}
END_TEST

START_TEST(test_front_on_empty_returns_null) {
    queue_t *q = new_queue(4);
    ck_assert_ptr_null(queue_front(q));
    free_queue(q);
}
END_TEST

START_TEST(test_back_on_empty_returns_null) {
    queue_t *q = new_queue(4);
    ck_assert_ptr_null(queue_back(q));
    free_queue(q);
}
END_TEST

START_TEST(test_front_does_not_remove_element) {
    queue_t *q = new_queue(4);
    queue_push(q, make_int(7));
    queue_front(q);
    queue_front(q);
    ck_assert_uint_eq(q->size, 1);
    free_elements(q);
    free_queue(q);
}
END_TEST

START_TEST(test_front_equals_back_when_single_element) {
    queue_t *q = new_queue(4);
    queue_push(q, make_int(55));
    ck_assert_ptr_eq(queue_front(q), queue_back(q));
    free_elements(q);
    free_queue(q);
}
END_TEST

/* ================================================================== */
/*  5. pop                                                             */
/* ================================================================== */

START_TEST(test_pop_returns_front_and_removes) {
    queue_t *q = new_queue(4);
    queue_push(q, make_int(1));
    queue_push(q, make_int(2));
    queue_push(q, make_int(3));
    void *val = queue_pop(q);
    ck_assert_ptr_nonnull(val);
    ck_assert_int_eq(*(int *)val, 1);
    ck_assert_uint_eq(q->size, 2);
    free(val);
    free_elements(q);
    free_queue(q);
}
END_TEST

START_TEST(test_pop_fifo_order) {
    queue_t *q = new_queue(4);
    queue_push(q, make_int(10));
    queue_push(q, make_int(20));
    queue_push(q, make_int(30));

    void *a = queue_pop(q);
    void *b = queue_pop(q);
    void *c = queue_pop(q);

    ck_assert_int_eq(*(int *)a, 10);
    ck_assert_int_eq(*(int *)b, 20);
    ck_assert_int_eq(*(int *)c, 30);

    free(a); free(b); free(c);
    free_queue(q);
}
END_TEST

START_TEST(test_pop_until_empty) {
    queue_t *q = new_queue(4);
    queue_push(q, make_int(1));
    queue_push(q, make_int(2));
    free(queue_pop(q));
    free(queue_pop(q));
    ck_assert(queue_is_empty(q));
    free_queue(q);
}
END_TEST

START_TEST(test_pop_on_empty_returns_null) {
    queue_t *q = new_queue(4);
    ck_assert_ptr_null(queue_pop(q));
    free_queue(q);
}
END_TEST

/* ================================================================== */
/*  6. Stress / combined operations                                    */
/* ================================================================== */

START_TEST(test_push_pop_interleaved) {
    queue_t *q = new_queue(4);
    for (int i = 0; i < 50; i++) {
        queue_push(q, make_int(i));
    }
    /* pop 25 — they should come out in FIFO order: 0..24 */
    for (int i = 0; i < 25; i++) {
        void *val = queue_pop(q);
        ck_assert_int_eq(*(int *)val, i);
        free(val);
    }
    ck_assert_uint_eq(q->size, 25);
    /* front of remaining is 25 */
    ck_assert_int_eq(*(int *)queue_front(q), 25);

    for (int i = 100; i < 125; i++) {
        queue_push(q, make_int(i));
    }
    ck_assert_uint_eq(q->size, 50);
    /* back should now be 124 */
    ck_assert_int_eq(*(int *)queue_back(q), 124);
    free_elements(q);
    free_queue(q);
}
END_TEST

START_TEST(test_large_queue) {
    queue_t *q = new_queue(16);
    int n = 100000;
    for (int i = 0; i < n; i++) {
        queue_push(q, make_int(i));
    }
    ck_assert_uint_eq(q->size, (size_t)n);
    ck_assert_int_eq(*(int *)queue_front(q), 0);
    ck_assert_int_eq(*(int *)queue_back(q), n - 1);
    free_elements(q);
    free_queue(q);
}
END_TEST

/* ================================================================== */
/*  Suite setup                                                        */
/* ================================================================== */

static Suite *queue_suite(void) {
    Suite *s = suite_create("queue");

    /* 1. Lifecycle */
    TCase *tc_lifecycle = tcase_create("lifecycle");
    tcase_add_test(tc_lifecycle, test_new_returns_valid_queue);
    tcase_add_test(tc_lifecycle, test_free_null_is_safe);
    suite_add_tcase(s, tc_lifecycle);

    /* 2. Empty / size */
    TCase *tc_empty = tcase_create("empty");
    tcase_add_test(tc_empty, test_new_queue_is_empty);
    tcase_add_test(tc_empty, test_not_empty_after_push);
    suite_add_tcase(s, tc_empty);

    /* 3. push */
    TCase *tc_push = tcase_create("push");
    tcase_add_test(tc_push, test_push_single);
    tcase_add_test(tc_push, test_push_multiple_fifo_order);
    tcase_add_test(tc_push, test_push_grows_capacity);
    suite_add_tcase(s, tc_push);

    /* 4. front / back */
    TCase *tc_access = tcase_create("front_back");
    tcase_add_test(tc_access, test_front_returns_oldest_element);
    tcase_add_test(tc_access, test_back_returns_newest_element);
    tcase_add_test(tc_access, test_front_on_empty_returns_null);
    tcase_add_test(tc_access, test_back_on_empty_returns_null);
    tcase_add_test(tc_access, test_front_does_not_remove_element);
    tcase_add_test(tc_access, test_front_equals_back_when_single_element);
    suite_add_tcase(s, tc_access);

    /* 5. pop */
    TCase *tc_pop = tcase_create("pop");
    tcase_add_test(tc_pop, test_pop_returns_front_and_removes);
    tcase_add_test(tc_pop, test_pop_fifo_order);
    tcase_add_test(tc_pop, test_pop_until_empty);
    tcase_add_test(tc_pop, test_pop_on_empty_returns_null);
    suite_add_tcase(s, tc_pop);

    /* 6. Stress */
    TCase *tc_stress = tcase_create("stress");
    tcase_set_timeout(tc_stress, 10);
    tcase_add_test(tc_stress, test_push_pop_interleaved);
    tcase_add_test(tc_stress, test_large_queue);
    suite_add_tcase(s, tc_stress);

    return s;
}

int main(void) {
    Suite *s = queue_suite();
    SRunner *sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    int nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
