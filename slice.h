#ifndef SLICE_H
#define SLICE_H

typedef struct
{
    int *data; // 2D slice가 시작하는 위치

    int rows; // 현재 slice의 행 길이
    int cols; // 현재 slice의 열 길이

    int row_cap; // 현재 시작 위치부터 아래 방향으로 사용할 수 있는 최대 행 수
    int col_cap; // 현재 시작 위치부터 오른쪽 방향으로 사용할 수 있는 최대 열 수

    int stride; // 실제 메모리에서 다음 행으로 이동하기 위한 간격
} Int2DSlice;

// 원본 2차원 배열 arr[row_low:row_high][col_low:col_high]를 slice로 만든다.
Int2DSlice slice2d(
    int *arr,
    int arr_rows,
    int arr_cols,
    int row_low,
    int row_high,
    int col_low,
    int col_high);

// 2D slice를 다시 자르기
Int2DSlice islice2d_slice(
    Int2DSlice s,
    int row_low,
    int row_high,
    int col_low,
    int col_high);

// 2D slice의 행, 열 개수 확인
int islice2d_rows(Int2DSlice s);
int islice2d_cols(Int2DSlice s);

// 2D slice의 capacity 확인
int islice2d_row_cap(Int2DSlice s);
int islice2d_col_cap(Int2DSlice s);

// 2D slice 값 읽기 / 변경
int islice2d_get(Int2DSlice s, int row, int col);
void islice2d_set(Int2DSlice s, int row, int col, int value);

/*
    행 하나 append

    Go에서 [][]int에 append를 하면
    바깥 slice에 새 행이 추가되는 것과 비슷하다.

    단, 이 구현은 직사각형 2D slice를 유지하기 위해
    새로 추가하는 행의 열 개수가 현재 cols와 같다고 가정한다.
*/
int islice2d_append_row(Int2DSlice *s, const int *row_values);

/*
    여러 행 append

    values는 n_rows x cols 크기의 값을
    1차원 배열에 row-major 순서로 저장한 것이다.

    단, 이 구현은 직사각형 2D slice를 유지하기 위해
    새로 추가하는 행들의 열 개수가 현재 cols와 같다고 가정한다.
*/
int islice2d_append_rows(Int2DSlice *s, const int *values, int n_rows);

// 2D slice 출력
void islice2d_print(Int2DSlice s);

#endif