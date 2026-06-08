#include <stdio.h>
#include <stdlib.h>
#include "slice.h"

static void slice_error(const char *message)
{
    printf("%s\n", message);
    exit(1);
}

Int2DSlice slice2d(
    int *arr,
    int arr_rows,
    int arr_cols,
    int row_low,
    int row_high,
    int col_low,
    int col_high)
{
    /*
        Hint:
        2차원 배열은 메모리에서 1차원처럼 연속 저장된다.

        arr[row][col]의 위치:
        row * arr_cols + col
    */

    /*
        Hint:
        stride는 원본 배열의 실제 열 개수이다.
        다음 행으로 이동할 때 stride만큼 이동해야 한다.
    */
}

Int2DSlice islice2d_slice(
    Int2DSlice s,
    int row_low,
    int row_high,
    int col_low,
    int col_high)
{
    /*
        Hint:
        기존 1차원 slice 구현과 맞추기 위해
        high는 현재 보이는 rows, cols가 아니라
        row_cap, col_cap 범위까지 허용한다.

        즉, 현재 보이는 영역보다 더 넓게 다시 slice할 수 있다.
    */

    /*
        Hint:
        여기서 s.cols가 아니라 s.stride를 사용해야 한다.

        s.cols는 현재 slice가 보는 열 개수이고,
        s.stride는 실제 한 행의 길이이다.
    */
}

int islice2d_rows(Int2DSlice s)
{
    // Hint: 현재 slice가 보는 행 개수
    return s.rows;
}

int islice2d_cols(Int2DSlice s)
{
    // Hint: 현재 slice가 보는 열 개수
    return s.cols;
}

int islice2d_row_cap(Int2DSlice s)
{
    // Hint: slice가 볼 수 있는 행의 총 개수
    return s.row_cap;
}

int islice2d_col_cap(Int2DSlice s)
{
    // Hint: slice가 볼 수 있는 열의 총 개수
    return s.col_cap;
}

int islice2d_get(Int2DSlice s, int row, int col)
{
    // Hint: s.data[row * s.stride + col]의 위치
    return s.data[row * s.stride + col];
}

void islice2d_set(Int2DSlice s, int row, int col, int value)
{
    // Hint: s.data[row * s.stride + col]의 위치
    s.data[row * s.stride + col] = value;
}

int islice2d_append_row(Int2DSlice *s, const int *row_values)
{
    return islice2d_append_rows(s, row_values, 1);
}

int islice2d_append_rows(Int2DSlice *s, const int *values, int n_rows)
{
    // Hint: n_rows x cols 크기의 values 배열이 row-major 순서로 저장되어 있다고 가정한다.
}

void islice2d_print(Int2DSlice s)
{
    int i;
    int j;

    printf("[\n");

    for (i = 0; i < s.rows; i++)
    {
        printf("  [");

        for (j = 0; j < s.cols; j++)
        {
            printf("%d", s.data[i * s.stride + j]);

            if (j < s.cols - 1)
            {
                printf(", ");
            }
        }

        printf("]\n");
    }

    printf("]\n");
}