#include <stdio.h>
#include "slice.h"

static int total_tests = 0;
static int passed_tests = 0;

static void check(const char *test_name, int condition)
{
    total_tests++;

    if (condition)
    {
        passed_tests++;
        printf("[O] %s\n", test_name);
    }
    else
    {
        printf("[X] %s\n", test_name);
    }
}

static int slice_equals(Int2DSlice s, const int *expected, int rows, int cols)
{
    int i;
    int j;

    if (islice2d_rows(s) != rows)
    {
        return 0;
    }

    if (islice2d_cols(s) != cols)
    {
        return 0;
    }

    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols; j++)
        {
            if (islice2d_get(s, i, j) != expected[i * cols + j])
            {
                return 0;
            }
        }
    }

    return 1;
}

static void print_array2d(const char *name, int *arr, int rows, int cols)
{
    int i;
    int j;

    printf("%s = \n", name);
    printf("[\n");

    for (i = 0; i < rows; i++)
    {
        printf("  [");

        for (j = 0; j < cols; j++)
        {
            printf("%d", arr[i * cols + j]);

            if (j < cols - 1)
            {
                printf(", ");
            }
        }

        printf("]\n");
    }

    printf("]\n");
}

int main(void)
{
    int arr[4][5] = {
        {1, 2, 3, 4, 5},
        {11, 12, 13, 14, 15},
        {21, 22, 23, 24, 25},
        {31, 32, 33, 34, 35}};

    Int2DSlice s;
    Int2DSlice sub;

    int expected1[] = {
        12, 13, 14,
        22, 23, 24};

    int expected2[] = {
        12, 13, 14,
        22, 23, 888};

    int expected3[] = {
        12, 13, 14, 15,
        22, 23, 888, 25,
        32, 33, 34, 35};

    int append_row_values[] = {
        100, 101, 102};

    int expected4[] = {
        12, 13, 14,
        22, 23, 888,
        100, 101, 102};

    int append_rows_values[] = {
        200, 201, 202,
        300, 301, 302};

    int expected5[] = {
        12, 13, 14,
        22, 23, 888,
        100, 101, 102,
        200, 201, 202,
        300, 301, 302};

    printf("=== Original Array ===\n");
    print_array2d("arr", &arr[0][0], 4, 5);

    printf("\n=== Test 1: slice2d ===\n");

    /*
        arr[1:3][1:4]를 slice로 만든다.

        예상:
        [
          [12, 13, 14]
          [22, 23, 24]
        ]

        rows = 2
        cols = 3
        row_cap = 3
        col_cap = 4
    */
    s = slice2d(&arr[0][0], 4, 5, 1, 3, 1, 4);

    check("slice2d rows", islice2d_rows(s) == 2);
    check("slice2d cols", islice2d_cols(s) == 3);
    check("slice2d row_cap", islice2d_row_cap(s) == 3);
    check("slice2d col_cap", islice2d_col_cap(s) == 4);
    check("slice2d values", slice_equals(s, expected1, 2, 3));

    printf("\nCurrent s:\n");
    islice2d_print(s);

    printf("\n=== Test 2: get / set ===\n");

    /*
        s[1][2]는 원본 배열 기준 arr[2][3]이다.
        따라서 s[1][2]를 바꾸면 arr[2][3]도 바뀌어야 한다.
    */
    check("islice2d_get before set", islice2d_get(s, 1, 2) == 24);

    islice2d_set(s, 1, 2, 888);

    check("islice2d_get after set", islice2d_get(s, 1, 2) == 888);
    check("set changes original array", arr[2][3] == 888);
    check("slice values after set", slice_equals(s, expected2, 2, 3));

    printf("\nCurrent s:\n");
    islice2d_print(s);

    printf("\n=== Test 3: slice of slice ===\n");

    /*
        현재 s의 보이는 크기는 2 x 3이다.
        하지만 capacity는 3 x 4이다.

        따라서 s에서 0:3, 0:4 범위로 다시 slice할 수 있어야 한다.
    */
    sub = islice2d_slice(s, 0, 3, 0, 4);

    check("islice2d_slice rows", islice2d_rows(sub) == 3);
    check("islice2d_slice cols", islice2d_cols(sub) == 4);
    check("islice2d_slice row_cap", islice2d_row_cap(sub) == 3);
    check("islice2d_slice col_cap", islice2d_col_cap(sub) == 4);
    check("islice2d_slice values", slice_equals(sub, expected3, 3, 4));

    printf("\nCurrent sub:\n");
    islice2d_print(sub);

    printf("\n=== Test 4: append one row within capacity ===\n");

    /*
        현재 s는 rows = 2, row_cap = 3이다.
        따라서 행 하나 append는 capacity 안에서 수행된다.

        이 경우 새 배열을 만들지 않고,
        원본 배열 arr에 값이 반영되어야 한다.
    */
    check("islice2d_append_row return",
          islice2d_append_row(&s, append_row_values) == 0);

    check("append_row rows", islice2d_rows(s) == 3);
    check("append_row cols", islice2d_cols(s) == 3);
    check("append_row row_cap unchanged", islice2d_row_cap(s) == 3);
    check("append_row col_cap unchanged", islice2d_col_cap(s) == 4);
    check("append_row values", slice_equals(s, expected4, 3, 3));

    check("append_row changes original arr[3][1]", arr[3][1] == 100);
    check("append_row changes original arr[3][2]", arr[3][2] == 101);
    check("append_row changes original arr[3][3]", arr[3][3] == 102);

    printf("\nCurrent s:\n");
    islice2d_print(s);

    printf("\n=== Test 5: append rows exceeding capacity ===\n");

    /*
        현재 s는 rows = 3, row_cap = 3이다.
        여기서 2행을 추가하면 capacity가 부족하다.

        따라서 malloc으로 새 배열을 만들고,
        기존 slice 값과 새 값을 복사해야 한다.
    */
    check("islice2d_append_rows return",
          islice2d_append_rows(&s, append_rows_values, 2) == 0);

    check("append_rows rows", islice2d_rows(s) == 5);
    check("append_rows cols", islice2d_cols(s) == 3);
    check("append_rows row_cap", islice2d_row_cap(s) == 6);
    check("append_rows col_cap", islice2d_col_cap(s) == 4);
    check("append_rows values", slice_equals(s, expected5, 5, 3));

    printf("\nCurrent s:\n");
    islice2d_print(s);

    printf("\n=== Test 6: after capacity exceeded, slice is separated from original array ===\n");

    /*
        capacity 초과 append 이후에는 s가 새 배열을 바라본다.
        따라서 s[0][0]을 바꿔도 원본 arr[1][1]은 바뀌면 안 된다.
    */
    islice2d_set(s, 0, 0, 555);

    check("set after capacity exceeded changes s", islice2d_get(s, 0, 0) == 555);
    check("set after capacity exceeded does not change original array", arr[1][1] == 12);

    printf("\nFinal s:\n");
    islice2d_print(s);

    printf("\nFinal arr:\n");
    print_array2d("arr", &arr[0][0], 4, 5);

    printf("\n=== Test Result ===\n");
    printf("%d / %d tests passed\n", passed_tests, total_tests);

    if (passed_tests == total_tests)
    {
        printf("All tests passed: O\n");
    }
    else
    {
        printf("Some tests failed: X\n");
    }

    /*
        주의:
        현재 Int2DSlice에는 owns_data 같은 필드가 없으므로
        s.data가 원본 배열을 가리키는지, malloc으로 만든 배열을 가리키는지
        일반적으로 구분할 수 없다.

        따라서 테스트 코드에서는 free(s.data)를 하지 않는다.
    */

    return 0;
}