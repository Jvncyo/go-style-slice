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
    if(arr == NULL) slice_error("Array is null\n");
    if(row_high < row_low || row_high < 0 || row_low < 0 || row_high > arr_rows || row_low > arr_rows) slice_error("Row values are wrong\n");
    if(col_high < col_low || col_high < 0 || col_low < 0 || col_high > arr_cols || col_low > arr_cols) slice_error("Column values are wrong\n");

    int rows = row_high - row_low;
    int cols = col_high - col_low;
    if(arr_rows < rows) slice_error("Range of Row is too large\n");
    if(arr_cols < cols) slice_error("Range of Column is too large\n");
    /*
        Hint:
        2차원 배열은 메모리에서 1차원처럼 연속 저장된다.

        arr[row][col]의 위치:
        row * arr_cols + col
    */
    int stride = arr_cols;
    int row_cap = arr_rows - row_low;
    int col_cap = arr_cols - col_low;
    /*
        Hint:
        stride는 원본 배열의 실제 열 개수이다.
        다음 행으로 이동할 때 stride만큼 이동해야 한다.
    */
    int *data = arr + (row_low * stride) + col_low;
    Int2DSlice slice = {data, rows, cols, row_cap, col_cap, stride};
    return slice;
}

Int2DSlice islice2d_slice(
    Int2DSlice s,
    int row_low,
    int row_high,
    int col_low,
    int col_high)
{
    if(row_high < row_low || row_high < 0 || row_low < 0 || row_high > s.row_cap || row_low > s.row_cap) slice_error("Row values are wrong\n");
    if(col_high < col_low || col_high < 0 || col_low < 0 || col_high > s.col_cap || col_low > s.col_cap) slice_error("Column values are wrong\n");

    int rows = row_high - row_low;
    int cols = col_high - col_low;
    if(s.row_cap < rows) slice_error("Range of Row is too large\n");
    if(s.col_cap < cols) slice_error("Range of Column is too large\n");
    /*
        Hint:
        기존 1차원 slice 구현과 맞추기 위해
        high는 현재 보이는 rows, cols가 아니라
        row_cap, col_cap 범위까지 허용한다.

        즉, 현재 보이는 영역보다 더 넓게 다시 slice할 수 있다.
    */
    int row_cap = s.row_cap - row_low;
    int col_cap = s.col_cap - col_low;
    /*
        Hint:
        여기서 s.cols가 아니라 s.stride를 사용해야 한다.

        s.cols는 현재 slice가 보는 열 개수이고,
        s.stride는 실제 한 행의 길이이다.
    */
   int *data = s.data + (row_low * s.stride) + col_low;
   Int2DSlice slice = {data, rows, cols, row_cap, col_cap, s.stride};
   return slice;
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
    if(s == NULL) return -1;
    int new_cap; //new row_cap
    int required_rows = s->rows + n_rows;

    if(s->row_cap >= required_rows) {
        for(int i = 0; i < n_rows; i++) {
            for(int j = 0; j < s->cols; j++) {
                int idx = (s->stride * i) + j;
                *(s->data + (s->stride * s->rows) + idx) = values[(s->cols * i) + j];
            }
        }
        s->rows += n_rows;
        return 0;
    } else {
        if(s->rows == 0) {
            new_cap = 1;
        } else {
            new_cap = s->rows;
        }

        while(new_cap < required_rows) {
            new_cap = new_cap * 2;
        }

        int *newData = malloc(sizeof(int) * new_cap * s->col_cap);

        for(int i = 0; i < s->rows; i++) {
            for(int j = 0; j < s->col_cap; j++) {
                newData[(i * s->col_cap) + j] = s->data[(s->stride * i) + j];
            }
        }
        for(int i = 0; i < n_rows; i++) {
            for(int j = 0; j < s->cols; j++) {
                int idx = (s->col_cap * i) + j;
                newData[(s->col_cap * s->rows) + idx] = values[(s->cols * i) + j];
            }
        }
        s->row_cap = new_cap;
        s->rows += n_rows;
        s->stride = s->col_cap;
        s->data = newData;
        return 0;
    }

    return -1;
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