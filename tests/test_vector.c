#include <check.h>
#include <clib/vector.h>
#include <stdlib.h>
#include <string.h>

/* ================================================================== */
/*  1. Lifecycle                                                      */
/* ================================================================== */

START_TEST(test_new_returns_valid_vector) {
    clib_vector *v = clib_vector_new(sizeof(int));
    ck_assert_ptr_nonnull(v);
    ck_assert_uint_eq(clib_vector_size(v), 0);
    (void)clib_vector_capacity(v);
    ck_assert_uint_eq(v->elem_size, sizeof(int));
    clib_vector_free(v);
}
END_TEST

START_TEST(test_new_different_elem_sizes) {
    clib_vector *v1 = clib_vector_new(sizeof(char));
    clib_vector *v2 = clib_vector_new(sizeof(double));

    typedef struct { int x; int y; int z; } point3d;
    clib_vector *v3 = clib_vector_new(sizeof(point3d));

    ck_assert_ptr_nonnull(v1);
    ck_assert_ptr_nonnull(v2);
    ck_assert_ptr_nonnull(v3);
    ck_assert_uint_eq(v1->elem_size, sizeof(char));
    ck_assert_uint_eq(v2->elem_size, sizeof(double));
    ck_assert_uint_eq(v3->elem_size, sizeof(point3d));

    clib_vector_free(v3);
    clib_vector_free(v2);
    clib_vector_free(v1);
}
END_TEST

START_TEST(test_free_null_is_safe) {
    clib_vector_free(NULL);
}
END_TEST

/* ================================================================== */
/*  2. Empty / size / capacity basics                                 */
/* ================================================================== */

START_TEST(test_new_vector_is_empty) {
    clib_vector *v = clib_vector_new(sizeof(int));
    ck_assert(clib_vector_empty(v));
    ck_assert_uint_eq(clib_vector_size(v), 0);
    clib_vector_free(v);
}
END_TEST

START_TEST(test_not_empty_after_push) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int val = 42;
    clib_vector_push_back(v, &val);
    ck_assert(!clib_vector_empty(v));
    ck_assert_uint_eq(clib_vector_size(v), 1);
    clib_vector_free(v);
}
END_TEST

/* ================================================================== */
/*  3. push_back                                                      */
/* ================================================================== */

START_TEST(test_push_back_single) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int val = 10;
    int rc = clib_vector_push_back(v, &val);
    ck_assert_int_eq(rc, 0);
    ck_assert_uint_eq(clib_vector_size(v), 1);
    ck_assert_int_eq(*(int *)clib_vector_at(v, 0), 10);
    clib_vector_free(v);
}
END_TEST

START_TEST(test_push_back_multiple) {
    clib_vector *v = clib_vector_new(sizeof(int));
    for (int i = 0; i < 100; i++) {
        ck_assert_int_eq(clib_vector_push_back(v, &i), 0);
    }
    ck_assert_uint_eq(clib_vector_size(v), 100);
    for (int i = 0; i < 100; i++) {
        ck_assert_int_eq(*(int *)clib_vector_at(v, (size_t)i), i);
    }
    clib_vector_free(v);
}
END_TEST

START_TEST(test_push_back_grows_capacity) {
    clib_vector *v = clib_vector_new(sizeof(int));
    size_t prev_cap = clib_vector_capacity(v);
    for (int i = 0; i < 64; i++) {
        clib_vector_push_back(v, &i);
    }
    ck_assert_uint_ge(clib_vector_capacity(v), 64);
    ck_assert_uint_ge(clib_vector_capacity(v), prev_cap);
    clib_vector_free(v);
}
END_TEST

START_TEST(test_push_back_preserves_data_after_grow) {
    clib_vector *v = clib_vector_new(sizeof(int));
    for (int i = 0; i < 1000; i++) {
        clib_vector_push_back(v, &i);
    }
    for (int i = 0; i < 1000; i++) {
        ck_assert_int_eq(*(int *)clib_vector_at(v, (size_t)i), i);
    }
    clib_vector_free(v);
}
END_TEST

START_TEST(test_push_back_doubles) {
    clib_vector *v = clib_vector_new(sizeof(double));
    for (int i = 0; i < 50; i++) {
        double d = i * 1.5;
        ck_assert_int_eq(clib_vector_push_back(v, &d), 0);
    }
    ck_assert_uint_eq(clib_vector_size(v), 50);
    for (int i = 0; i < 50; i++) {
        double expected = i * 1.5;
        double actual = *(double *)clib_vector_at(v, (size_t)i);
        ck_assert_double_eq(actual, expected);
    }
    clib_vector_free(v);
}
END_TEST

/* ================================================================== */
/*  4. Element access — at, front, back, data                         */
/* ================================================================== */

START_TEST(test_at_returns_correct_element) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int vals[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) {
        clib_vector_push_back(v, &vals[i]);
    }
    for (int i = 0; i < 5; i++) {
        ck_assert_int_eq(*(int *)clib_vector_at(v, (size_t)i), vals[i]);
    }
    clib_vector_free(v);
}
END_TEST

START_TEST(test_at_out_of_bounds_returns_null) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int val = 1;
    clib_vector_push_back(v, &val);
    ck_assert_ptr_null(clib_vector_at(v, 1));
    ck_assert_ptr_null(clib_vector_at(v, 100));
    clib_vector_free(v);
}
END_TEST

START_TEST(test_at_on_empty_returns_null) {
    clib_vector *v = clib_vector_new(sizeof(int));
    ck_assert_ptr_null(clib_vector_at(v, 0));
    clib_vector_free(v);
}
END_TEST

START_TEST(test_front_returns_first_element) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int vals[] = {100, 200, 300};
    for (int i = 0; i < 3; i++) {
        clib_vector_push_back(v, &vals[i]);
    }
    ck_assert_int_eq(*(int *)clib_vector_front(v), 100);
    clib_vector_free(v);
}
END_TEST

START_TEST(test_front_on_empty_returns_null) {
    clib_vector *v = clib_vector_new(sizeof(int));
    ck_assert_ptr_null(clib_vector_front(v));
    clib_vector_free(v);
}
END_TEST

START_TEST(test_back_returns_last_element) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int vals[] = {100, 200, 300};
    for (int i = 0; i < 3; i++) {
        clib_vector_push_back(v, &vals[i]);
    }
    ck_assert_int_eq(*(int *)clib_vector_back(v), 300);
    clib_vector_free(v);
}
END_TEST

START_TEST(test_back_on_empty_returns_null) {
    clib_vector *v = clib_vector_new(sizeof(int));
    ck_assert_ptr_null(clib_vector_back(v));
    clib_vector_free(v);
}
END_TEST

START_TEST(test_data_returns_raw_pointer) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int vals[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) {
        clib_vector_push_back(v, &vals[i]);
    }
    int *raw = (int *)clib_vector_data(v);
    ck_assert_ptr_nonnull(raw);
    ck_assert_int_eq(raw[0], 1);
    ck_assert_int_eq(raw[1], 2);
    ck_assert_int_eq(raw[2], 3);
    clib_vector_free(v);
}
END_TEST

START_TEST(test_at_returns_mutable_pointer) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int val = 5;
    clib_vector_push_back(v, &val);
    *(int *)clib_vector_at(v, 0) = 99;
    ck_assert_int_eq(*(int *)clib_vector_at(v, 0), 99);
    clib_vector_free(v);
}
END_TEST

/* ================================================================== */
/*  5. pop_back                                                       */
/* ================================================================== */

START_TEST(test_pop_back_decrements_size) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int vals[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) {
        clib_vector_push_back(v, &vals[i]);
    }
    ck_assert_int_eq(clib_vector_pop_back(v), 0);
    ck_assert_uint_eq(clib_vector_size(v), 2);
    ck_assert_int_eq(*(int *)clib_vector_back(v), 2);
    clib_vector_free(v);
}
END_TEST

START_TEST(test_pop_back_until_empty) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int vals[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) {
        clib_vector_push_back(v, &vals[i]);
    }
    ck_assert_int_eq(clib_vector_pop_back(v), 0);
    ck_assert_int_eq(clib_vector_pop_back(v), 0);
    ck_assert_int_eq(clib_vector_pop_back(v), 0);
    ck_assert(clib_vector_empty(v));
    clib_vector_free(v);
}
END_TEST

START_TEST(test_pop_back_on_empty_returns_error) {
    clib_vector *v = clib_vector_new(sizeof(int));
    ck_assert_int_ne(clib_vector_pop_back(v), 0);
    clib_vector_free(v);
}
END_TEST

/* ================================================================== */
/*  6. insert                                                         */
/* ================================================================== */

START_TEST(test_insert_at_beginning) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int vals[] = {2, 3};
    for (int i = 0; i < 2; i++) {
        clib_vector_push_back(v, &vals[i]);
    }
    int one = 1;
    ck_assert_int_eq(clib_vector_insert(v, 0, &one), 0);
    ck_assert_uint_eq(clib_vector_size(v), 3);
    ck_assert_int_eq(*(int *)clib_vector_at(v, 0), 1);
    ck_assert_int_eq(*(int *)clib_vector_at(v, 1), 2);
    ck_assert_int_eq(*(int *)clib_vector_at(v, 2), 3);
    clib_vector_free(v);
}
END_TEST

START_TEST(test_insert_at_middle) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int vals[] = {1, 3};
    for (int i = 0; i < 2; i++) {
        clib_vector_push_back(v, &vals[i]);
    }
    int two = 2;
    ck_assert_int_eq(clib_vector_insert(v, 1, &two), 0);
    ck_assert_uint_eq(clib_vector_size(v), 3);
    ck_assert_int_eq(*(int *)clib_vector_at(v, 0), 1);
    ck_assert_int_eq(*(int *)clib_vector_at(v, 1), 2);
    ck_assert_int_eq(*(int *)clib_vector_at(v, 2), 3);
    clib_vector_free(v);
}
END_TEST

START_TEST(test_insert_at_end) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int vals[] = {1, 2};
    for (int i = 0; i < 2; i++) {
        clib_vector_push_back(v, &vals[i]);
    }
    int three = 3;
    ck_assert_int_eq(clib_vector_insert(v, 2, &three), 0);
    ck_assert_uint_eq(clib_vector_size(v), 3);
    ck_assert_int_eq(*(int *)clib_vector_back(v), 3);
    clib_vector_free(v);
}
END_TEST

START_TEST(test_insert_out_of_bounds_returns_error) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int val = 1;
    clib_vector_push_back(v, &val);
    int x = 99;
    ck_assert_int_ne(clib_vector_insert(v, 5, &x), 0);
    ck_assert_uint_eq(clib_vector_size(v), 1);
    clib_vector_free(v);
}
END_TEST

START_TEST(test_insert_into_empty_vector) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int val = 42;
    ck_assert_int_eq(clib_vector_insert(v, 0, &val), 0);
    ck_assert_uint_eq(clib_vector_size(v), 1);
    ck_assert_int_eq(*(int *)clib_vector_at(v, 0), 42);
    clib_vector_free(v);
}
END_TEST

/* ================================================================== */
/*  7. erase                                                          */
/* ================================================================== */

START_TEST(test_erase_first) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int vals[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) {
        clib_vector_push_back(v, &vals[i]);
    }
    ck_assert_int_eq(clib_vector_erase(v, 0), 0);
    ck_assert_uint_eq(clib_vector_size(v), 2);
    ck_assert_int_eq(*(int *)clib_vector_at(v, 0), 2);
    ck_assert_int_eq(*(int *)clib_vector_at(v, 1), 3);
    clib_vector_free(v);
}
END_TEST

START_TEST(test_erase_middle) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int vals[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) {
        clib_vector_push_back(v, &vals[i]);
    }
    ck_assert_int_eq(clib_vector_erase(v, 1), 0);
    ck_assert_uint_eq(clib_vector_size(v), 2);
    ck_assert_int_eq(*(int *)clib_vector_at(v, 0), 1);
    ck_assert_int_eq(*(int *)clib_vector_at(v, 1), 3);
    clib_vector_free(v);
}
END_TEST

START_TEST(test_erase_last) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int vals[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) {
        clib_vector_push_back(v, &vals[i]);
    }
    ck_assert_int_eq(clib_vector_erase(v, 2), 0);
    ck_assert_uint_eq(clib_vector_size(v), 2);
    ck_assert_int_eq(*(int *)clib_vector_back(v), 2);
    clib_vector_free(v);
}
END_TEST

START_TEST(test_erase_out_of_bounds_returns_error) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int val = 1;
    clib_vector_push_back(v, &val);
    ck_assert_int_ne(clib_vector_erase(v, 1), 0);
    ck_assert_int_ne(clib_vector_erase(v, 100), 0);
    ck_assert_uint_eq(clib_vector_size(v), 1);
    clib_vector_free(v);
}
END_TEST

START_TEST(test_erase_on_empty_returns_error) {
    clib_vector *v = clib_vector_new(sizeof(int));
    ck_assert_int_ne(clib_vector_erase(v, 0), 0);
    clib_vector_free(v);
}
END_TEST

/* ================================================================== */
/*  8. reserve                                                        */
/* ================================================================== */

START_TEST(test_reserve_increases_capacity) {
    clib_vector *v = clib_vector_new(sizeof(int));
    ck_assert_int_eq(clib_vector_reserve(v, 100), 0);
    ck_assert_uint_ge(clib_vector_capacity(v), 100);
    ck_assert_uint_eq(clib_vector_size(v), 0);
    clib_vector_free(v);
}
END_TEST

START_TEST(test_reserve_smaller_is_noop) {
    clib_vector *v = clib_vector_new(sizeof(int));
    clib_vector_reserve(v, 100);
    size_t cap = clib_vector_capacity(v);
    clib_vector_reserve(v, 10);
    ck_assert_uint_eq(clib_vector_capacity(v), cap);
    clib_vector_free(v);
}
END_TEST

START_TEST(test_reserve_preserves_existing_data) {
    clib_vector *v = clib_vector_new(sizeof(int));
    for (int i = 0; i < 5; i++) {
        clib_vector_push_back(v, &i);
    }
    clib_vector_reserve(v, 500);
    ck_assert_uint_eq(clib_vector_size(v), 5);
    for (int i = 0; i < 5; i++) {
        ck_assert_int_eq(*(int *)clib_vector_at(v, (size_t)i), i);
    }
    clib_vector_free(v);
}
END_TEST

/* ================================================================== */
/*  9. shrink_to_fit                                                  */
/* ================================================================== */

START_TEST(test_shrink_to_fit_reduces_capacity) {
    clib_vector *v = clib_vector_new(sizeof(int));
    clib_vector_reserve(v, 1000);
    for (int i = 0; i < 5; i++) {
        clib_vector_push_back(v, &i);
    }
    ck_assert_uint_ge(clib_vector_capacity(v), 1000);
    clib_vector_shrink_to_fit(v);
    ck_assert_uint_le(clib_vector_capacity(v), 1000);
    ck_assert_uint_ge(clib_vector_capacity(v), clib_vector_size(v));
    for (int i = 0; i < 5; i++) {
        ck_assert_int_eq(*(int *)clib_vector_at(v, (size_t)i), i);
    }
    clib_vector_free(v);
}
END_TEST

START_TEST(test_shrink_to_fit_on_empty) {
    clib_vector *v = clib_vector_new(sizeof(int));
    clib_vector_reserve(v, 100);
    clib_vector_shrink_to_fit(v);
    ck_assert_uint_eq(clib_vector_size(v), 0);
    ck_assert_uint_le(clib_vector_capacity(v), 1);
    clib_vector_free(v);
}
END_TEST

/* ================================================================== */
/*  10. clear                                                         */
/* ================================================================== */

START_TEST(test_clear_resets_size) {
    clib_vector *v = clib_vector_new(sizeof(int));
    for (int i = 0; i < 10; i++) {
        clib_vector_push_back(v, &i);
    }
    clib_vector_clear(v);
    ck_assert_uint_eq(clib_vector_size(v), 0);
    ck_assert(clib_vector_empty(v));
    clib_vector_free(v);
}
END_TEST

START_TEST(test_clear_does_not_reduce_capacity) {
    clib_vector *v = clib_vector_new(sizeof(int));
    for (int i = 0; i < 50; i++) {
        clib_vector_push_back(v, &i);
    }
    size_t cap = clib_vector_capacity(v);
    clib_vector_clear(v);
    ck_assert_uint_eq(clib_vector_capacity(v), cap);
    clib_vector_free(v);
}
END_TEST

START_TEST(test_push_after_clear) {
    clib_vector *v = clib_vector_new(sizeof(int));
    for (int i = 0; i < 5; i++) {
        clib_vector_push_back(v, &i);
    }
    clib_vector_clear(v);
    int val = 99;
    clib_vector_push_back(v, &val);
    ck_assert_uint_eq(clib_vector_size(v), 1);
    ck_assert_int_eq(*(int *)clib_vector_at(v, 0), 99);
    clib_vector_free(v);
}
END_TEST

/* ================================================================== */
/*  11. resize                                                        */
/* ================================================================== */

START_TEST(test_resize_grow_zero_fills) {
    clib_vector *v = clib_vector_new(sizeof(int));
    for (int i = 1; i <= 3; i++) {
        clib_vector_push_back(v, &i);
    }
    ck_assert_int_eq(clib_vector_resize(v, 6), 0);
    ck_assert_uint_eq(clib_vector_size(v), 6);
    ck_assert_int_eq(*(int *)clib_vector_at(v, 0), 1);
    ck_assert_int_eq(*(int *)clib_vector_at(v, 1), 2);
    ck_assert_int_eq(*(int *)clib_vector_at(v, 2), 3);
    ck_assert_int_eq(*(int *)clib_vector_at(v, 3), 0);
    ck_assert_int_eq(*(int *)clib_vector_at(v, 4), 0);
    ck_assert_int_eq(*(int *)clib_vector_at(v, 5), 0);
    clib_vector_free(v);
}
END_TEST

START_TEST(test_resize_shrink) {
    clib_vector *v = clib_vector_new(sizeof(int));
    for (int i = 0; i < 10; i++) {
        clib_vector_push_back(v, &i);
    }
    ck_assert_int_eq(clib_vector_resize(v, 3), 0);
    ck_assert_uint_eq(clib_vector_size(v), 3);
    ck_assert_int_eq(*(int *)clib_vector_at(v, 0), 0);
    ck_assert_int_eq(*(int *)clib_vector_at(v, 1), 1);
    ck_assert_int_eq(*(int *)clib_vector_at(v, 2), 2);
    clib_vector_free(v);
}
END_TEST

START_TEST(test_resize_to_zero) {
    clib_vector *v = clib_vector_new(sizeof(int));
    for (int i = 0; i < 5; i++) {
        clib_vector_push_back(v, &i);
    }
    ck_assert_int_eq(clib_vector_resize(v, 0), 0);
    ck_assert(clib_vector_empty(v));
    clib_vector_free(v);
}
END_TEST

START_TEST(test_resize_same_size_is_noop) {
    clib_vector *v = clib_vector_new(sizeof(int));
    for (int i = 0; i < 5; i++) {
        clib_vector_push_back(v, &i);
    }
    ck_assert_int_eq(clib_vector_resize(v, 5), 0);
    ck_assert_uint_eq(clib_vector_size(v), 5);
    for (int i = 0; i < 5; i++) {
        ck_assert_int_eq(*(int *)clib_vector_at(v, (size_t)i), i);
    }
    clib_vector_free(v);
}
END_TEST

/* ================================================================== */
/*  12. Struct element type                                           */
/* ================================================================== */

typedef struct {
    int   id;
    float score;
    char  name[16];
} record;

START_TEST(test_push_back_structs) {
    clib_vector *v = clib_vector_new(sizeof(record));

    record r1 = { .id = 1, .score = 95.5f, .name = "Alice" };
    record r2 = { .id = 2, .score = 87.0f, .name = "Bob" };
    record r3 = { .id = 3, .score = 72.3f, .name = "Charlie" };

    clib_vector_push_back(v, &r1);
    clib_vector_push_back(v, &r2);
    clib_vector_push_back(v, &r3);

    ck_assert_uint_eq(clib_vector_size(v), 3);

    record *p = (record *)clib_vector_at(v, 1);
    ck_assert_ptr_nonnull(p);
    ck_assert_int_eq(p->id, 2);
    ck_assert_float_eq(p->score, 87.0f);
    ck_assert_str_eq(p->name, "Bob");

    clib_vector_free(v);
}
END_TEST

START_TEST(test_insert_erase_structs) {
    clib_vector *v = clib_vector_new(sizeof(record));

    record r1 = { .id = 1, .score = 1.0f, .name = "A" };
    record r2 = { .id = 2, .score = 2.0f, .name = "B" };
    record r3 = { .id = 3, .score = 3.0f, .name = "C" };

    clib_vector_push_back(v, &r1);
    clib_vector_push_back(v, &r3);
    clib_vector_insert(v, 1, &r2);

    ck_assert_uint_eq(clib_vector_size(v), 3);
    ck_assert_int_eq(((record *)clib_vector_at(v, 0))->id, 1);
    ck_assert_int_eq(((record *)clib_vector_at(v, 1))->id, 2);
    ck_assert_int_eq(((record *)clib_vector_at(v, 2))->id, 3);

    clib_vector_erase(v, 1);
    ck_assert_uint_eq(clib_vector_size(v), 2);
    ck_assert_int_eq(((record *)clib_vector_at(v, 0))->id, 1);
    ck_assert_int_eq(((record *)clib_vector_at(v, 1))->id, 3);

    clib_vector_free(v);
}
END_TEST

/* ================================================================== */
/*  13. Stress / combined operations                                  */
/* ================================================================== */

START_TEST(test_push_pop_interleaved) {
    clib_vector *v = clib_vector_new(sizeof(int));
    for (int i = 0; i < 50; i++) {
        clib_vector_push_back(v, &i);
    }
    for (int i = 0; i < 25; i++) {
        clib_vector_pop_back(v);
    }
    ck_assert_uint_eq(clib_vector_size(v), 25);
    for (int i = 0; i < 25; i++) {
        ck_assert_int_eq(*(int *)clib_vector_at(v, (size_t)i), i);
    }
    for (int i = 100; i < 150; i++) {
        clib_vector_push_back(v, &i);
    }
    ck_assert_uint_eq(clib_vector_size(v), 75);
    ck_assert_int_eq(*(int *)clib_vector_at(v, 25), 100);
    clib_vector_free(v);
}
END_TEST

START_TEST(test_large_vector) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int n = 100000;
    for (int i = 0; i < n; i++) {
        ck_assert_int_eq(clib_vector_push_back(v, &i), 0);
    }
    ck_assert_uint_eq(clib_vector_size(v), (size_t)n);
    ck_assert_int_eq(*(int *)clib_vector_front(v), 0);
    ck_assert_int_eq(*(int *)clib_vector_back(v), n - 1);
    ck_assert_int_eq(*(int *)clib_vector_at(v, 50000), 50000);
    ck_assert_int_eq(*(int *)clib_vector_at(v, 99999), 99999);
    clib_vector_free(v);
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
    tcase_add_test(tc_lifecycle, test_new_different_elem_sizes);
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
    tcase_add_test(tc_push, test_push_back_doubles);
    suite_add_tcase(s, tc_push);

    /* 4. Element access */
    TCase *tc_access = tcase_create("element_access");
    tcase_add_test(tc_access, test_at_returns_correct_element);
    tcase_add_test(tc_access, test_at_out_of_bounds_returns_null);
    tcase_add_test(tc_access, test_at_on_empty_returns_null);
    tcase_add_test(tc_access, test_front_returns_first_element);
    tcase_add_test(tc_access, test_front_on_empty_returns_null);
    tcase_add_test(tc_access, test_back_returns_last_element);
    tcase_add_test(tc_access, test_back_on_empty_returns_null);
    tcase_add_test(tc_access, test_data_returns_raw_pointer);
    tcase_add_test(tc_access, test_at_returns_mutable_pointer);
    suite_add_tcase(s, tc_access);

    /* 5. pop_back */
    TCase *tc_pop = tcase_create("pop_back");
    tcase_add_test(tc_pop, test_pop_back_decrements_size);
    tcase_add_test(tc_pop, test_pop_back_until_empty);
    tcase_add_test(tc_pop, test_pop_back_on_empty_returns_error);
    suite_add_tcase(s, tc_pop);

    /* 6. insert */
    TCase *tc_insert = tcase_create("insert");
    tcase_add_test(tc_insert, test_insert_at_beginning);
    tcase_add_test(tc_insert, test_insert_at_middle);
    tcase_add_test(tc_insert, test_insert_at_end);
    tcase_add_test(tc_insert, test_insert_out_of_bounds_returns_error);
    tcase_add_test(tc_insert, test_insert_into_empty_vector);
    suite_add_tcase(s, tc_insert);

    /* 7. erase */
    TCase *tc_erase = tcase_create("erase");
    tcase_add_test(tc_erase, test_erase_first);
    tcase_add_test(tc_erase, test_erase_middle);
    tcase_add_test(tc_erase, test_erase_last);
    tcase_add_test(tc_erase, test_erase_out_of_bounds_returns_error);
    tcase_add_test(tc_erase, test_erase_on_empty_returns_error);
    suite_add_tcase(s, tc_erase);

    /* 8. reserve */
    TCase *tc_reserve = tcase_create("reserve");
    tcase_add_test(tc_reserve, test_reserve_increases_capacity);
    tcase_add_test(tc_reserve, test_reserve_smaller_is_noop);
    tcase_add_test(tc_reserve, test_reserve_preserves_existing_data);
    suite_add_tcase(s, tc_reserve);

    /* 9. shrink_to_fit */
    TCase *tc_shrink = tcase_create("shrink_to_fit");
    tcase_add_test(tc_shrink, test_shrink_to_fit_reduces_capacity);
    tcase_add_test(tc_shrink, test_shrink_to_fit_on_empty);
    suite_add_tcase(s, tc_shrink);

    /* 10. clear */
    TCase *tc_clear = tcase_create("clear");
    tcase_add_test(tc_clear, test_clear_resets_size);
    tcase_add_test(tc_clear, test_clear_does_not_reduce_capacity);
    tcase_add_test(tc_clear, test_push_after_clear);
    suite_add_tcase(s, tc_clear);

    /* 11. resize */
    TCase *tc_resize = tcase_create("resize");
    tcase_add_test(tc_resize, test_resize_grow_zero_fills);
    tcase_add_test(tc_resize, test_resize_shrink);
    tcase_add_test(tc_resize, test_resize_to_zero);
    tcase_add_test(tc_resize, test_resize_same_size_is_noop);
    suite_add_tcase(s, tc_resize);

    /* 12. Struct elements */
    TCase *tc_struct = tcase_create("struct_elements");
    tcase_add_test(tc_struct, test_push_back_structs);
    tcase_add_test(tc_struct, test_insert_erase_structs);
    suite_add_tcase(s, tc_struct);

    /* 13. Stress */
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
