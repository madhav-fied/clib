#include <clib/vector.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/*  Minimal test harness                                              */
/* ------------------------------------------------------------------ */

static int tests_run    = 0;
static int tests_passed = 0;
static int tests_failed = 0;

#define ASSERT(cond)                                                         \
    do {                                                                     \
        if (!(cond)) {                                                       \
            printf("  FAIL: %s  (%s:%d)\n", #cond, __FILE__, __LINE__);      \
            return 1;                                                        \
        }                                                                    \
    } while (0)

#define ASSERT_EQ(a, b)   ASSERT((a) == (b))
#define ASSERT_NEQ(a, b)  ASSERT((a) != (b))
#define ASSERT_NULL(p)    ASSERT((p) == NULL)
#define ASSERT_NOT_NULL(p) ASSERT((p) != NULL)

#define RUN_TEST(fn)                                                         \
    do {                                                                     \
        printf("%-50s", #fn);                                                \
        tests_run++;                                                         \
        if (fn() == 0) { tests_passed++; printf("PASS\n"); }                \
        else           { tests_failed++; }                                   \
    } while (0)

/* ================================================================== */
/*  1. Lifecycle                                                      */
/* ================================================================== */

static int test_new_returns_valid_vector(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    ASSERT_NOT_NULL(v);
    ASSERT_EQ(clib_vector_size(v), 0);
    (void)clib_vector_capacity(v);  /* just verify it doesn't crash */
    ASSERT_EQ(v->elem_size, sizeof(int));
    clib_vector_free(v);
    return 0;
}

static int test_new_different_elem_sizes(void) {
    clib_vector *v1 = clib_vector_new(sizeof(char));
    clib_vector *v2 = clib_vector_new(sizeof(double));

    typedef struct { int x; int y; int z; } point3d;
    clib_vector *v3 = clib_vector_new(sizeof(point3d));

    ASSERT_NOT_NULL(v1);
    ASSERT_NOT_NULL(v2);
    ASSERT_NOT_NULL(v3);
    ASSERT_EQ(v1->elem_size, sizeof(char));
    ASSERT_EQ(v2->elem_size, sizeof(double));
    ASSERT_EQ(v3->elem_size, sizeof(point3d));

    clib_vector_free(v3);
    clib_vector_free(v2);
    clib_vector_free(v1);
    return 0;
}

static int test_free_null_is_safe(void) {
    /* Calling free(NULL) must not crash — mirrors free() semantics. */
    clib_vector_free(NULL);
    return 0;
}

/* ================================================================== */
/*  2. Empty / size / capacity basics                                 */
/* ================================================================== */

static int test_new_vector_is_empty(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    ASSERT(clib_vector_empty(v));
    ASSERT_EQ(clib_vector_size(v), 0);
    clib_vector_free(v);
    return 0;
}

static int test_not_empty_after_push(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int val = 42;
    clib_vector_push_back(v, &val);
    ASSERT(!clib_vector_empty(v));
    ASSERT_EQ(clib_vector_size(v), 1);
    clib_vector_free(v);
    return 0;
}

/* ================================================================== */
/*  3. push_back                                                      */
/* ================================================================== */

static int test_push_back_single(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int val = 10;
    int rc = clib_vector_push_back(v, &val);
    ASSERT_EQ(rc, 0);
    ASSERT_EQ(clib_vector_size(v), 1);
    ASSERT_EQ(*(int *)clib_vector_at(v, 0), 10);
    clib_vector_free(v);
    return 0;
}

static int test_push_back_multiple(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    for (int i = 0; i < 100; i++) {
        ASSERT_EQ(clib_vector_push_back(v, &i), 0);
    }
    ASSERT_EQ(clib_vector_size(v), 100);
    for (int i = 0; i < 100; i++) {
        ASSERT_EQ(*(int *)clib_vector_at(v, (size_t)i), i);
    }
    clib_vector_free(v);
    return 0;
}

static int test_push_back_grows_capacity(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    size_t prev_cap = clib_vector_capacity(v);
    /* Push enough elements to force at least one reallocation. */
    for (int i = 0; i < 64; i++) {
        clib_vector_push_back(v, &i);
    }
    ASSERT(clib_vector_capacity(v) >= 64);
    ASSERT(clib_vector_capacity(v) >= prev_cap);
    clib_vector_free(v);
    return 0;
}

static int test_push_back_preserves_data_after_grow(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    for (int i = 0; i < 1000; i++) {
        clib_vector_push_back(v, &i);
    }
    /* Verify all data is intact after many reallocations. */
    for (int i = 0; i < 1000; i++) {
        ASSERT_EQ(*(int *)clib_vector_at(v, (size_t)i), i);
    }
    clib_vector_free(v);
    return 0;
}

static int test_push_back_doubles(void) {
    clib_vector *v = clib_vector_new(sizeof(double));
    for (int i = 0; i < 50; i++) {
        double d = i * 1.5;
        ASSERT_EQ(clib_vector_push_back(v, &d), 0);
    }
    ASSERT_EQ(clib_vector_size(v), 50);
    for (int i = 0; i < 50; i++) {
        double expected = i * 1.5;
        double actual   = *(double *)clib_vector_at(v, (size_t)i);
        ASSERT(actual == expected);
    }
    clib_vector_free(v);
    return 0;
}

/* ================================================================== */
/*  4. Element access — at, front, back, data                         */
/* ================================================================== */

static int test_at_returns_correct_element(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int vals[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) {
        clib_vector_push_back(v, &vals[i]);
    }
    for (int i = 0; i < 5; i++) {
        ASSERT_EQ(*(int *)clib_vector_at(v, (size_t)i), vals[i]);
    }
    clib_vector_free(v);
    return 0;
}

static int test_at_out_of_bounds_returns_null(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int val = 1;
    clib_vector_push_back(v, &val);

    ASSERT_NULL(clib_vector_at(v, 1));
    ASSERT_NULL(clib_vector_at(v, 100));
    clib_vector_free(v);
    return 0;
}

static int test_at_on_empty_returns_null(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    ASSERT_NULL(clib_vector_at(v, 0));
    clib_vector_free(v);
    return 0;
}

static int test_front_returns_first_element(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int vals[] = {100, 200, 300};
    for (int i = 0; i < 3; i++) {
        clib_vector_push_back(v, &vals[i]);
    }
    ASSERT_EQ(*(int *)clib_vector_front(v), 100);
    clib_vector_free(v);
    return 0;
}

static int test_front_on_empty_returns_null(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    ASSERT_NULL(clib_vector_front(v));
    clib_vector_free(v);
    return 0;
}

static int test_back_returns_last_element(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int vals[] = {100, 200, 300};
    for (int i = 0; i < 3; i++) {
        clib_vector_push_back(v, &vals[i]);
    }
    ASSERT_EQ(*(int *)clib_vector_back(v), 300);
    clib_vector_free(v);
    return 0;
}

static int test_back_on_empty_returns_null(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    ASSERT_NULL(clib_vector_back(v));
    clib_vector_free(v);
    return 0;
}

static int test_data_returns_raw_pointer(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int vals[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) {
        clib_vector_push_back(v, &vals[i]);
    }
    int *raw = (int *)clib_vector_data(v);
    ASSERT_NOT_NULL(raw);
    ASSERT_EQ(raw[0], 1);
    ASSERT_EQ(raw[1], 2);
    ASSERT_EQ(raw[2], 3);
    clib_vector_free(v);
    return 0;
}

static int test_at_returns_mutable_pointer(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int val = 5;
    clib_vector_push_back(v, &val);
    /* Modify element through the returned pointer. */
    *(int *)clib_vector_at(v, 0) = 99;
    ASSERT_EQ(*(int *)clib_vector_at(v, 0), 99);
    clib_vector_free(v);
    return 0;
}

/* ================================================================== */
/*  5. pop_back                                                       */
/* ================================================================== */

static int test_pop_back_decrements_size(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int vals[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) {
        clib_vector_push_back(v, &vals[i]);
    }
    ASSERT_EQ(clib_vector_pop_back(v), 0);
    ASSERT_EQ(clib_vector_size(v), 2);
    ASSERT_EQ(*(int *)clib_vector_back(v), 2);
    clib_vector_free(v);
    return 0;
}

static int test_pop_back_until_empty(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int vals[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) {
        clib_vector_push_back(v, &vals[i]);
    }
    ASSERT_EQ(clib_vector_pop_back(v), 0);
    ASSERT_EQ(clib_vector_pop_back(v), 0);
    ASSERT_EQ(clib_vector_pop_back(v), 0);
    ASSERT(clib_vector_empty(v));
    clib_vector_free(v);
    return 0;
}

static int test_pop_back_on_empty_returns_error(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    ASSERT_NEQ(clib_vector_pop_back(v), 0);
    clib_vector_free(v);
    return 0;
}

/* ================================================================== */
/*  6. insert                                                         */
/* ================================================================== */

static int test_insert_at_beginning(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int vals[] = {2, 3};
    for (int i = 0; i < 2; i++) {
        clib_vector_push_back(v, &vals[i]);
    }
    int one = 1;
    ASSERT_EQ(clib_vector_insert(v, 0, &one), 0);
    ASSERT_EQ(clib_vector_size(v), 3);
    ASSERT_EQ(*(int *)clib_vector_at(v, 0), 1);
    ASSERT_EQ(*(int *)clib_vector_at(v, 1), 2);
    ASSERT_EQ(*(int *)clib_vector_at(v, 2), 3);
    clib_vector_free(v);
    return 0;
}

static int test_insert_at_middle(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int vals[] = {1, 3};
    for (int i = 0; i < 2; i++) {
        clib_vector_push_back(v, &vals[i]);
    }
    int two = 2;
    ASSERT_EQ(clib_vector_insert(v, 1, &two), 0);
    ASSERT_EQ(clib_vector_size(v), 3);
    ASSERT_EQ(*(int *)clib_vector_at(v, 0), 1);
    ASSERT_EQ(*(int *)clib_vector_at(v, 1), 2);
    ASSERT_EQ(*(int *)clib_vector_at(v, 2), 3);
    clib_vector_free(v);
    return 0;
}

static int test_insert_at_end(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int vals[] = {1, 2};
    for (int i = 0; i < 2; i++) {
        clib_vector_push_back(v, &vals[i]);
    }
    int three = 3;
    /* Inserting at index == size is equivalent to push_back. */
    ASSERT_EQ(clib_vector_insert(v, 2, &three), 0);
    ASSERT_EQ(clib_vector_size(v), 3);
    ASSERT_EQ(*(int *)clib_vector_back(v), 3);
    clib_vector_free(v);
    return 0;
}

static int test_insert_out_of_bounds_returns_error(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int val = 1;
    clib_vector_push_back(v, &val);
    int x = 99;
    /* Index 5 is beyond size (which is 1). */
    ASSERT_NEQ(clib_vector_insert(v, 5, &x), 0);
    ASSERT_EQ(clib_vector_size(v), 1);
    clib_vector_free(v);
    return 0;
}

static int test_insert_into_empty_vector(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int val = 42;
    ASSERT_EQ(clib_vector_insert(v, 0, &val), 0);
    ASSERT_EQ(clib_vector_size(v), 1);
    ASSERT_EQ(*(int *)clib_vector_at(v, 0), 42);
    clib_vector_free(v);
    return 0;
}

/* ================================================================== */
/*  7. erase                                                          */
/* ================================================================== */

static int test_erase_first(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int vals[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) {
        clib_vector_push_back(v, &vals[i]);
    }
    ASSERT_EQ(clib_vector_erase(v, 0), 0);
    ASSERT_EQ(clib_vector_size(v), 2);
    ASSERT_EQ(*(int *)clib_vector_at(v, 0), 2);
    ASSERT_EQ(*(int *)clib_vector_at(v, 1), 3);
    clib_vector_free(v);
    return 0;
}

static int test_erase_middle(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int vals[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) {
        clib_vector_push_back(v, &vals[i]);
    }
    ASSERT_EQ(clib_vector_erase(v, 1), 0);
    ASSERT_EQ(clib_vector_size(v), 2);
    ASSERT_EQ(*(int *)clib_vector_at(v, 0), 1);
    ASSERT_EQ(*(int *)clib_vector_at(v, 1), 3);
    clib_vector_free(v);
    return 0;
}

static int test_erase_last(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int vals[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) {
        clib_vector_push_back(v, &vals[i]);
    }
    ASSERT_EQ(clib_vector_erase(v, 2), 0);
    ASSERT_EQ(clib_vector_size(v), 2);
    ASSERT_EQ(*(int *)clib_vector_back(v), 2);
    clib_vector_free(v);
    return 0;
}

static int test_erase_out_of_bounds_returns_error(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int val = 1;
    clib_vector_push_back(v, &val);
    ASSERT_NEQ(clib_vector_erase(v, 1), 0);
    ASSERT_NEQ(clib_vector_erase(v, 100), 0);
    ASSERT_EQ(clib_vector_size(v), 1);
    clib_vector_free(v);
    return 0;
}

static int test_erase_on_empty_returns_error(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    ASSERT_NEQ(clib_vector_erase(v, 0), 0);
    clib_vector_free(v);
    return 0;
}

/* ================================================================== */
/*  8. reserve                                                        */
/* ================================================================== */

static int test_reserve_increases_capacity(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    ASSERT_EQ(clib_vector_reserve(v, 100), 0);
    ASSERT(clib_vector_capacity(v) >= 100);
    ASSERT_EQ(clib_vector_size(v), 0);   /* size unchanged */
    clib_vector_free(v);
    return 0;
}

static int test_reserve_smaller_is_noop(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    clib_vector_reserve(v, 100);
    size_t cap = clib_vector_capacity(v);
    /* Reserving smaller does nothing. */
    clib_vector_reserve(v, 10);
    ASSERT_EQ(clib_vector_capacity(v), cap);
    clib_vector_free(v);
    return 0;
}

static int test_reserve_preserves_existing_data(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    for (int i = 0; i < 5; i++) {
        clib_vector_push_back(v, &i);
    }
    clib_vector_reserve(v, 500);
    ASSERT_EQ(clib_vector_size(v), 5);
    for (int i = 0; i < 5; i++) {
        ASSERT_EQ(*(int *)clib_vector_at(v, (size_t)i), i);
    }
    clib_vector_free(v);
    return 0;
}

/* ================================================================== */
/*  9. shrink_to_fit                                                  */
/* ================================================================== */

static int test_shrink_to_fit_reduces_capacity(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    clib_vector_reserve(v, 1000);
    for (int i = 0; i < 5; i++) {
        clib_vector_push_back(v, &i);
    }
    ASSERT(clib_vector_capacity(v) >= 1000);
    clib_vector_shrink_to_fit(v);
    /* After shrink, capacity should equal size (or at least be smaller). */
    ASSERT(clib_vector_capacity(v) <= 1000);
    ASSERT(clib_vector_capacity(v) >= clib_vector_size(v));
    /* Data must still be intact. */
    for (int i = 0; i < 5; i++) {
        ASSERT_EQ(*(int *)clib_vector_at(v, (size_t)i), i);
    }
    clib_vector_free(v);
    return 0;
}

static int test_shrink_to_fit_on_empty(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    clib_vector_reserve(v, 100);
    clib_vector_shrink_to_fit(v);
    ASSERT_EQ(clib_vector_size(v), 0);
    /* Capacity should drop — at least not exceed a small number. */
    ASSERT(clib_vector_capacity(v) <= 1);
    clib_vector_free(v);
    return 0;
}

/* ================================================================== */
/*  10. clear                                                         */
/* ================================================================== */

static int test_clear_resets_size(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    for (int i = 0; i < 10; i++) {
        clib_vector_push_back(v, &i);
    }
    clib_vector_clear(v);
    ASSERT_EQ(clib_vector_size(v), 0);
    ASSERT(clib_vector_empty(v));
    clib_vector_free(v);
    return 0;
}

static int test_clear_does_not_reduce_capacity(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    for (int i = 0; i < 50; i++) {
        clib_vector_push_back(v, &i);
    }
    size_t cap = clib_vector_capacity(v);
    clib_vector_clear(v);
    /* clear() should NOT release memory — capacity stays. */
    ASSERT_EQ(clib_vector_capacity(v), cap);
    clib_vector_free(v);
    return 0;
}

static int test_push_after_clear(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    for (int i = 0; i < 5; i++) {
        clib_vector_push_back(v, &i);
    }
    clib_vector_clear(v);
    int val = 99;
    clib_vector_push_back(v, &val);
    ASSERT_EQ(clib_vector_size(v), 1);
    ASSERT_EQ(*(int *)clib_vector_at(v, 0), 99);
    clib_vector_free(v);
    return 0;
}

/* ================================================================== */
/*  11. resize                                                        */
/* ================================================================== */

static int test_resize_grow_zero_fills(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    for (int i = 1; i <= 3; i++) {
        clib_vector_push_back(v, &i);
    }
    ASSERT_EQ(clib_vector_resize(v, 6), 0);
    ASSERT_EQ(clib_vector_size(v), 6);
    /* Original elements unchanged. */
    ASSERT_EQ(*(int *)clib_vector_at(v, 0), 1);
    ASSERT_EQ(*(int *)clib_vector_at(v, 1), 2);
    ASSERT_EQ(*(int *)clib_vector_at(v, 2), 3);
    /* New elements zero-initialized. */
    ASSERT_EQ(*(int *)clib_vector_at(v, 3), 0);
    ASSERT_EQ(*(int *)clib_vector_at(v, 4), 0);
    ASSERT_EQ(*(int *)clib_vector_at(v, 5), 0);
    clib_vector_free(v);
    return 0;
}

static int test_resize_shrink(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    for (int i = 0; i < 10; i++) {
        clib_vector_push_back(v, &i);
    }
    ASSERT_EQ(clib_vector_resize(v, 3), 0);
    ASSERT_EQ(clib_vector_size(v), 3);
    ASSERT_EQ(*(int *)clib_vector_at(v, 0), 0);
    ASSERT_EQ(*(int *)clib_vector_at(v, 1), 1);
    ASSERT_EQ(*(int *)clib_vector_at(v, 2), 2);
    clib_vector_free(v);
    return 0;
}

static int test_resize_to_zero(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    for (int i = 0; i < 5; i++) {
        clib_vector_push_back(v, &i);
    }
    ASSERT_EQ(clib_vector_resize(v, 0), 0);
    ASSERT(clib_vector_empty(v));
    clib_vector_free(v);
    return 0;
}

static int test_resize_same_size_is_noop(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    for (int i = 0; i < 5; i++) {
        clib_vector_push_back(v, &i);
    }
    ASSERT_EQ(clib_vector_resize(v, 5), 0);
    ASSERT_EQ(clib_vector_size(v), 5);
    for (int i = 0; i < 5; i++) {
        ASSERT_EQ(*(int *)clib_vector_at(v, (size_t)i), i);
    }
    clib_vector_free(v);
    return 0;
}

/* ================================================================== */
/*  12. Struct element type                                           */
/* ================================================================== */

typedef struct {
    int   id;
    float score;
    char  name[16];
} record;

static int test_push_back_structs(void) {
    clib_vector *v = clib_vector_new(sizeof(record));

    record r1 = { .id = 1, .score = 95.5f, .name = "Alice" };
    record r2 = { .id = 2, .score = 87.0f, .name = "Bob" };
    record r3 = { .id = 3, .score = 72.3f, .name = "Charlie" };

    clib_vector_push_back(v, &r1);
    clib_vector_push_back(v, &r2);
    clib_vector_push_back(v, &r3);

    ASSERT_EQ(clib_vector_size(v), 3);

    record *p = (record *)clib_vector_at(v, 1);
    ASSERT_NOT_NULL(p);
    ASSERT_EQ(p->id, 2);
    ASSERT(p->score == 87.0f);
    ASSERT(strcmp(p->name, "Bob") == 0);

    clib_vector_free(v);
    return 0;
}

static int test_insert_erase_structs(void) {
    clib_vector *v = clib_vector_new(sizeof(record));

    record r1 = { .id = 1, .score = 1.0f, .name = "A" };
    record r2 = { .id = 2, .score = 2.0f, .name = "B" };
    record r3 = { .id = 3, .score = 3.0f, .name = "C" };

    clib_vector_push_back(v, &r1);
    clib_vector_push_back(v, &r3);
    clib_vector_insert(v, 1, &r2);

    ASSERT_EQ(clib_vector_size(v), 3);
    ASSERT_EQ(((record *)clib_vector_at(v, 0))->id, 1);
    ASSERT_EQ(((record *)clib_vector_at(v, 1))->id, 2);
    ASSERT_EQ(((record *)clib_vector_at(v, 2))->id, 3);

    clib_vector_erase(v, 1);
    ASSERT_EQ(clib_vector_size(v), 2);
    ASSERT_EQ(((record *)clib_vector_at(v, 0))->id, 1);
    ASSERT_EQ(((record *)clib_vector_at(v, 1))->id, 3);

    clib_vector_free(v);
    return 0;
}

/* ================================================================== */
/*  13. Stress / combined operations                                  */
/* ================================================================== */

static int test_push_pop_interleaved(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    for (int i = 0; i < 50; i++) {
        clib_vector_push_back(v, &i);
    }
    for (int i = 0; i < 25; i++) {
        clib_vector_pop_back(v);
    }
    ASSERT_EQ(clib_vector_size(v), 25);
    for (int i = 0; i < 25; i++) {
        ASSERT_EQ(*(int *)clib_vector_at(v, (size_t)i), i);
    }
    /* Push more again. */
    for (int i = 100; i < 150; i++) {
        clib_vector_push_back(v, &i);
    }
    ASSERT_EQ(clib_vector_size(v), 75);
    ASSERT_EQ(*(int *)clib_vector_at(v, 25), 100);
    clib_vector_free(v);
    return 0;
}

static int test_large_vector(void) {
    clib_vector *v = clib_vector_new(sizeof(int));
    int n = 100000;
    for (int i = 0; i < n; i++) {
        ASSERT_EQ(clib_vector_push_back(v, &i), 0);
    }
    ASSERT_EQ(clib_vector_size(v), (size_t)n);
    ASSERT_EQ(*(int *)clib_vector_front(v), 0);
    ASSERT_EQ(*(int *)clib_vector_back(v), n - 1);
    /* Spot-check some values. */
    ASSERT_EQ(*(int *)clib_vector_at(v, 50000), 50000);
    ASSERT_EQ(*(int *)clib_vector_at(v, 99999), 99999);
    clib_vector_free(v);
    return 0;
}

/* ================================================================== */
/*  main                                                              */
/* ================================================================== */

int main(void) {
    printf("\n=== clib_vector test suite ===\n\n");

    /* 1. Lifecycle */
    RUN_TEST(test_new_returns_valid_vector);
    RUN_TEST(test_new_different_elem_sizes);
    RUN_TEST(test_free_null_is_safe);

    /* 2. Empty / size */
    RUN_TEST(test_new_vector_is_empty);
    RUN_TEST(test_not_empty_after_push);

    /* 3. push_back */
    RUN_TEST(test_push_back_single);
    RUN_TEST(test_push_back_multiple);
    RUN_TEST(test_push_back_grows_capacity);
    RUN_TEST(test_push_back_preserves_data_after_grow);
    RUN_TEST(test_push_back_doubles);

    /* 4. Element access */
    RUN_TEST(test_at_returns_correct_element);
    RUN_TEST(test_at_out_of_bounds_returns_null);
    RUN_TEST(test_at_on_empty_returns_null);
    RUN_TEST(test_front_returns_first_element);
    RUN_TEST(test_front_on_empty_returns_null);
    RUN_TEST(test_back_returns_last_element);
    RUN_TEST(test_back_on_empty_returns_null);
    RUN_TEST(test_data_returns_raw_pointer);
    RUN_TEST(test_at_returns_mutable_pointer);

    /* 5. pop_back */
    RUN_TEST(test_pop_back_decrements_size);
    RUN_TEST(test_pop_back_until_empty);
    RUN_TEST(test_pop_back_on_empty_returns_error);

    /* 6. insert */
    RUN_TEST(test_insert_at_beginning);
    RUN_TEST(test_insert_at_middle);
    RUN_TEST(test_insert_at_end);
    RUN_TEST(test_insert_out_of_bounds_returns_error);
    RUN_TEST(test_insert_into_empty_vector);

    /* 7. erase */
    RUN_TEST(test_erase_first);
    RUN_TEST(test_erase_middle);
    RUN_TEST(test_erase_last);
    RUN_TEST(test_erase_out_of_bounds_returns_error);
    RUN_TEST(test_erase_on_empty_returns_error);

    /* 8. reserve */
    RUN_TEST(test_reserve_increases_capacity);
    RUN_TEST(test_reserve_smaller_is_noop);
    RUN_TEST(test_reserve_preserves_existing_data);

    /* 9. shrink_to_fit */
    RUN_TEST(test_shrink_to_fit_reduces_capacity);
    RUN_TEST(test_shrink_to_fit_on_empty);

    /* 10. clear */
    RUN_TEST(test_clear_resets_size);
    RUN_TEST(test_clear_does_not_reduce_capacity);
    RUN_TEST(test_push_after_clear);

    /* 11. resize */
    RUN_TEST(test_resize_grow_zero_fills);
    RUN_TEST(test_resize_shrink);
    RUN_TEST(test_resize_to_zero);
    RUN_TEST(test_resize_same_size_is_noop);

    /* 12. Struct elements */
    RUN_TEST(test_push_back_structs);
    RUN_TEST(test_insert_erase_structs);

    /* 13. Stress */
    RUN_TEST(test_push_pop_interleaved);
    RUN_TEST(test_large_vector);

    /* Summary */
    printf("\n---------------------------------\n");
    printf("Total: %d | Passed: %d | Failed: %d\n\n", tests_run, tests_passed, tests_failed);

    return tests_failed > 0 ? 1 : 0;
}
