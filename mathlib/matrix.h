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

// �ж����,ÿһ��ǳ��ӽ�ʱ,��Ϊ���
uint8_t Matrix_Equal(Matrix* m1, Matrix* m2, float tolerance);

// copy: src -> dst
uint8_t Matrix_Copy(Matrix* src, Matrix* dst);

// out = m1 + m2; outҲ����Ϊm1��m2
uint8_t Matrix_Add(Matrix* m1, Matrix* m2, Matrix* out);

// out = m1 - m2; outҲ����Ϊm1��m2
uint8_t Matrix_Sub(Matrix* m1, Matrix* m2, Matrix* out);

// m *= s; out����Ϊm
void Matrix_Scale(Matrix* m, float scale, Matrix* out);

// out = m1 * m2; out����Ϊm1 or m2
uint8_t Matrix_Multiply(Matrix* m1, Matrix* m2, Matrix* out);

// out = m^T; out����Ϊm
uint8_t Matrix_Transpose(Matrix* m, Matrix* out);

// m = m^T
uint8_t Matrix_SelfTranspose(Matrix* m);

// out = m^-1;ʹ��Gauss-Jordan method��m�ᱻ�޸�ΪI;out����Ϊm
uint8_t Matrix_Inverse(Matrix* m, Matrix* out);

//tostring
char* Matrix_ToString(Matrix* m, char* buff);

//----------header end--------------

#ifdef __cplusplus
}
#endif


