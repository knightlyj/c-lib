#pragma once

#ifdef __cplusplus
extern "C" {
#endif

//---------header start---------------

#include "mathex.h"
#include "stdint.h"

typedef struct {
    uint8_t row, col;
    float* data;
} Matrix;

//define a matrix with a data array
#define MAT_DEF(name, rowSize, colSize) \
float name##_mat_data[rowSize * colSize]; \
Matrix name = {rowSize, colSize, name##_mat_data};

//indexer; M[r][c]
#define MAT_IDX(m, r, c) \
(m).data[r * (m).col + c]

// matrix = I 
uint8_t Matrix_Identity(Matrix* m);

// 判断相等,每一项都非常接近时,认为相等
uint8_t Matrix_Equal(Matrix* m1, Matrix* m2, float tolerance);

// copy: src -> dst
uint8_t Matrix_Copy(Matrix* src, Matrix* dst);

// out = m1 + m2; out也可以为m1或m2
uint8_t Matrix_Add(Matrix* m1, Matrix* m2, Matrix* out);

// out = m1 - m2; out也可以为m1或m2
uint8_t Matrix_Sub(Matrix* m1, Matrix* m2, Matrix* out);

// m *= s; out可以为m
void Matrix_Scale(Matrix* m, float scale, Matrix* out);

// out = m1 * m2; out不能为m1 or m2
uint8_t Matrix_Multiply(Matrix* m1, Matrix* m2, Matrix* out);

// out = m^T; out可以为m
uint8_t Matrix_Transpose(Matrix* m, Matrix* out);

// m = m^T
uint8_t Matrix_SelfTranspose(Matrix* m);

// out = m^-1;使用Gauss-Jordan method，m会被修改为I;out不可为m
uint8_t Matrix_Inverse(Matrix* m, Matrix* out);

//tostring
char* Matrix_ToString(Matrix* m, char* buff);

//----------header end--------------

#ifdef __cplusplus
}
#endif


