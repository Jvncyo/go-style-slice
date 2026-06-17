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
    // 예외처리
    if(arr == NULL) slice_error("Array is null\n");
    if(row_high < row_low || row_high < 0 || row_low < 0 || row_high > arr_rows || row_low > arr_rows) slice_error("Row values are wrong\n");
    if(col_high < col_low || col_high < 0 || col_low < 0 || col_high > arr_cols || col_low > arr_cols) slice_error("Column values are wrong\n");

    // slice row, col 계산
    int rows = row_high - row_low;
    int cols = col_high - col_low;
    // slice range 넘어갈 경우 예외처리
    if(arr_rows < rows) slice_error("Range of Row is too large\n");
    if(arr_cols < cols) slice_error("Range of Column is too large\n");
    
    int stride = arr_cols; // stride는 원본의 실제 열 개수
    // row_cap, col_cap 계산
    int row_cap = arr_rows - row_low;
    int col_cap = arr_cols - col_low;
    
    int *data = arr + (row_low * stride) + col_low; // slice한 새로운 배열의 시작주소 계산
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
    // 예외처리
    if(row_high < row_low || row_high < 0 || row_low < 0 || row_high > s.row_cap || row_low > s.row_cap) slice_error("Row values are wrong\n");
    if(col_high < col_low || col_high < 0 || col_low < 0 || col_high > s.col_cap || col_low > s.col_cap) slice_error("Column values are wrong\n");

    // 새로운 row, col 계산
    int rows = row_high - row_low;
    int cols = col_high - col_low;
    // slice range 넘어갈 경우 예외처리
    if(s.row_cap < rows) slice_error("Range of Row is too large\n");
    if(s.col_cap < cols) slice_error("Range of Column is too large\n");
    
    // 새로운 row_cap, col_cap 계산
    int row_cap = s.row_cap - row_low;
    int col_cap = s.col_cap - col_low;
    
    int *data = s.data + (row_low * s.stride) + col_low; // 새로운 시작주소 계산
    Int2DSlice slice = {data, rows, cols, row_cap, col_cap, s.stride};
    return slice;
}

int islice2d_rows(Int2DSlice s)
{
    // 현재 slice가 보는 행 개수
    return s.rows;
}

int islice2d_cols(Int2DSlice s)
{
    // 현재 slice가 보는 열 개수
    return s.cols;
}

int islice2d_row_cap(Int2DSlice s)
{
    // slice가 볼 수 있는 행의 총 개수
    return s.row_cap;
}

int islice2d_col_cap(Int2DSlice s)
{
    // slice가 볼 수 있는 열의 총 개수
    return s.col_cap;
}

int islice2d_get(Int2DSlice s, int row, int col)
{
    // s.data[row * s.stride + col]의 위치
    return s.data[row * s.stride + col];
}

void islice2d_set(Int2DSlice s, int row, int col, int value)
{
    // s.data[row * s.stride + col]의 위치 값 변경
    s.data[row * s.stride + col] = value;
}

int islice2d_append_row(Int2DSlice *s, const int *row_values)
{
    return islice2d_append_rows(s, row_values, 1);
}

int islice2d_append_rows(Int2DSlice *s, const int *values, int n_rows)
{
    // n_rows x cols 크기의 values 배열이 row-major 순서로 저장되어 있다고 가정한다.
    
    if(s == NULL || values == NULL || n_rows < 0) return -1; // 예외처리
    int new_cap; // new row_cap
    int required_rows = s->rows + n_rows; // 필요한 row 개수 설정

    if(s->row_cap >= required_rows) { // 남은 row 개수가 필요한 row 개수보다 많다면 
        for(int i = 0; i < n_rows; i++) { // 남는 공간에 values 값 붙여넣기
            for(int j = 0; j < s->cols; j++) {
                int idx = (s->stride * i) + j;
                *(s->data + (s->stride * s->rows) + idx) = values[(s->cols * i) + j];
            }
        }
        s->rows += n_rows;
        return 0;
    } else { // 남은 row 개수가 필요한 row 개수보다 적다면
        if(s->rows == 0) { 
            new_cap = 1;
        } else {
            new_cap = s->rows;
        }

        while(new_cap < required_rows) { // 새로운 row_cap 계산
            new_cap = new_cap * 2;
        }

        int *newData = malloc(sizeof(int) * new_cap * s->col_cap); // 계산한 new_cap으로 배열 생성

        for(int i = 0; i < s->rows; i++) { // 원래 있던 값 새로운 배열로 붙여넣기
            for(int j = 0; j < s->col_cap; j++) {
                newData[(i * s->col_cap) + j] = s->data[(s->stride * i) + j];
            }
        }
        for(int i = 0; i < n_rows; i++) { // values 값 새 배열로 붙여넣기
            for(int j = 0; j < s->cols; j++) {
                int idx = (s->col_cap * i) + j;
                newData[(s->col_cap * s->rows) + idx] = values[(s->cols * i) + j];
            }
        }

        // slice 구조체 변수 최신화
        s->row_cap = new_cap;
        s->rows += n_rows;
        s->stride = s->col_cap;
        s->data = newData;
        return 0;
    }

    return -1; // 만일을 위한 실패처리
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