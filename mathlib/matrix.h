#pragma once

#ifdef __cplusplus
extern "C" {
#endif

//---------header start---------------

#include "mathex.h"
#include "stdint.h"

#ifndef  MAT_FLOAT
#define MAT_FLOAT double
#endif // ! MAT_FLOAT

typedef struct {
    uint8_t row, col;
    MAT_FLOAT* data;
} Matrix;

//define a matrix with a data array
#define MAT_DEF(name, rowSize, colSize) \
MAT_FLOAT name##_mat_data[rowSize * colSize]; \
Matrix name = {rowSize, colSize, name##_mat_data};

//indexer; M[r][c]
#define MAT_IDX(m, r, c) \
((m).data[r * (m).col + c])

// matrix = I 
uint8_t Matrix_Identity(Matrix* m);

// �ж����,ÿһ��ǳ��ӽ�ʱ,��Ϊ���
uint8_t Matrix_Equal(Matrix* m1, Matrix* m2, MAT_FLOAT tolerance);

// copy: src -> dst
uint8_t Matrix_Copy(Matrix* src, Matrix* dst);

// out = m1 + m2; outҲ����Ϊm1��m2
uint8_t Matrix_Add(Matrix* m1, Matrix* m2, Matrix* out);

// out = m1 - m2; outҲ����Ϊm1��m2
uint8_t Matrix_Sub(Matrix* m1, Matrix* m2, Matrix* out);

// m *= s; out����Ϊm
void Matrix_Scale(Matrix* m, MAT_FLOAT scale, Matrix* out);

// out = m1 * m2; out����Ϊm1 or m2
uint8_t Matrix_Multiply(Matrix* m1, Matrix* m2, Matrix* out);

// out = m^T; out����Ϊm
uint8_t Matrix_Transpose(Matrix* m, Matrix* out);

// m = m^T
uint8_t Matrix_SelfTranspose(Matrix* m);

// out = m^-1;ʹ��Gauss-Jordan method��m�ᱻ�޸�ΪI;out����Ϊm
uint8_t Matrix_Inverse(Matrix* m, Matrix* out);

//tostring
char* Matrix_ToString(Matrix* m, char* buff, char* seperator);

//----------header end--------------

#ifdef __cplusplus
}
#endif


