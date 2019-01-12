#include "matrix.h"
#include "stdio.h"
#include "stdlib.h"
#include "assert.h"
#include "time.h"

#define MAX_TEST_SIZE 8

void IdentityTest(void)
{
    //正确数据测试
    for (int i = 1; i < MAX_TEST_SIZE; i++)
    {
        Matrix m;
        m.row = i;
        m.col = i;
        m.data = malloc(sizeof(float) * i * i);

        assert(Matrix_Identity(&m) != 0);

        for (int k = 0; k < m.row; k++)
        {
            for (int x = 0; x < m.col; x++)
            {
                if (x == k)
                {
                    assert(FLOAT_EQUAL(MAT_IDX(m, k, x), 1.0f));
                }
                else
                {
                    assert(FLOAT_EQUAL(MAT_IDX(m, k, x), 0.0f));
                }
            }
        }
        free(m.data);
    }

    for (int i = 1; i < MAX_TEST_SIZE; i++)
    {
        for (int k = 1; k < i; k++)
        {
            Matrix m;
            m.row = i;
            m.col = k;
            m.data = malloc(sizeof(float) * i * k);

            assert(Matrix_Identity(&m) == 0);

            m.row = k;
            m.col = i;

            assert(Matrix_Identity(&m) == 0);

            free(m.data);
        }
    }
}

void EqualTest(void)
{
    srand((uint32_t)time(NULL));

    for (int count = 0; count < 10; count++)
    {
        uint8_t rowSize, colSize;
        rowSize = (uint32_t)rand() % MAX_TEST_SIZE + 1;
        colSize = (uint32_t)rand() % MAX_TEST_SIZE + 1;

        Matrix m1;
        m1.row = rowSize;
        m1.col = colSize;
        m1.data = malloc(sizeof(float) * rowSize * colSize);

        Matrix m2;
        m2.row = rowSize;
        m2.col = colSize;
        m2.data = malloc(sizeof(float) * rowSize * colSize);

        for (int i = 0; i < rowSize; i++)
        {
            for (int k = 0; k < colSize; k++)
            {
                float r = (float)rand() / 20.0f;
                MAT_IDX(m1, i, k) = r;
                MAT_IDX(m2, i, k) = r;
            }
        }

        assert(Matrix_Equal(&m1, &m2, INFINIT_SMALL));

        uint8_t r, c;
        r = (uint32_t)rand() % rowSize;
        c = (uint32_t)rand() % colSize;
        MAT_IDX(m2, r, c) *= 1.001f;

        assert(!Matrix_Equal(&m1, &m2, INFINIT_SMALL));

        free(m1.data);
        free(m2.data);
    }
}


void CopyTest(void)
{
    for (int count = 0; count < 10; count++)
    {
        uint8_t rowSize, colSize;
        rowSize = (uint32_t)rand() % MAX_TEST_SIZE + 1;
        colSize = (uint32_t)rand() % MAX_TEST_SIZE + 1;

        Matrix m1;
        m1.row = rowSize;
        m1.col = colSize;
        m1.data = malloc(sizeof(float) * rowSize * colSize);

        Matrix m2;
        m2.row = rowSize;
        m2.col = colSize;
        m2.data = malloc(sizeof(float) * rowSize * colSize);

        for (int i = 0; i < rowSize; i++)
        {
            for (int k = 0; k < colSize; k++)
            {
                float r = (float)rand() / 20.0f;
                MAT_IDX(m1, i, k) = r;
            }
        }

        Matrix_Copy(&m1, &m2);
        
        assert(Matrix_Equal(&m1, &m2, INFINIT_SMALL));

        free(m1.data);
        free(m2.data);
    }
}

void AddTest(void)
{
    for (int count = 0; count < 10; count++)
    {
        uint8_t rowSize, colSize;
        rowSize = (uint32_t)rand() % MAX_TEST_SIZE + 1;
        colSize = (uint32_t)rand() % MAX_TEST_SIZE + 1;

        Matrix m1;
        m1.row = rowSize;
        m1.col = colSize;
        m1.data = malloc(sizeof(float) * rowSize * colSize);

        Matrix m2;
        m2.row = rowSize;
        m2.col = colSize;
        m2.data = malloc(sizeof(float) * rowSize * colSize);

        Matrix out;
        out.row = rowSize;
        out.col = colSize;
        out.data = malloc(sizeof(float) * rowSize * colSize);

        for (int i = 0; i < rowSize; i++)
        {
            for (int k = 0; k < colSize; k++)
            {
                float r = (float)rand() / 20.0f;
                MAT_IDX(m1, i, k) = r;
                r = (float)rand() / 20.0f;
                MAT_IDX(m2, i, k) = r;
            }
        }

        Matrix_Add(&m1, &m2, &out);

        for (int i = 0; i < rowSize; i++)
        {
            for (int k = 0; k < colSize; k++)
            {
                float sum = MAT_IDX(m1, i, k) + MAT_IDX(m2, i, k);
                assert(FLOAT_EQUAL(sum, MAT_IDX(out, i, k)));
            }
        }

        free(m1.data);
        free(m2.data);
        free(out.data);
    }
}

void SubTest(void)
{
    for (int count = 0; count < 10; count++)
    {
        uint8_t rowSize, colSize;
        rowSize = (uint32_t)rand() % MAX_TEST_SIZE + 1;
        colSize = (uint32_t)rand() % MAX_TEST_SIZE + 1;

        Matrix m1;
        m1.row = rowSize;
        m1.col = colSize;
        m1.data = malloc(sizeof(float) * rowSize * colSize);

        Matrix m2;
        m2.row = rowSize;
        m2.col = colSize;
        m2.data = malloc(sizeof(float) * rowSize * colSize);

        Matrix out;
        out.row = rowSize;
        out.col = colSize;
        out.data = malloc(sizeof(float) * rowSize * colSize);

        for (int i = 0; i < rowSize; i++)
        {
            for (int k = 0; k < colSize; k++)
            {
                float r = (float)rand() / 20.0f;
                MAT_IDX(m1, i, k) = r;
                r = (float)rand() / 20.0f;
                MAT_IDX(m2, i, k) = r;
            }
        }

        Matrix_Sub(&m1, &m2, &out);

        for (int i = 0; i < rowSize; i++)
        {
            for (int k = 0; k < colSize; k++)
            {
                float sum = MAT_IDX(m1, i, k) - MAT_IDX(m2, i, k);
                assert(FLOAT_EQUAL(sum, MAT_IDX(out, i, k)));
            }
        }

        free(m1.data);
        free(m2.data);
        free(out.data);
    }
}

void ScaleTest()
{
    for (int count = 0; count < 10; count++)
    {
        uint8_t rowSize, colSize;
        rowSize = (uint32_t)rand() % MAX_TEST_SIZE + 1;
        colSize = (uint32_t)rand() % MAX_TEST_SIZE + 1;

        Matrix m;
        m.row = rowSize;
        m.col = colSize;
        m.data = malloc(sizeof(float) * rowSize * colSize);

        Matrix out;
        out.row = rowSize;
        out.col = colSize;
        out.data = malloc(sizeof(float) * rowSize * colSize);

        for (int i = 0; i < rowSize; i++)
        {
            for (int k = 0; k < colSize; k++)
            {
                float r = (float)rand() / 20.0f;
                MAT_IDX(m, i, k) = r;
            }
        }

        float s = (float)rand() / 20.0f;
        Matrix_Scale(&m, s, &out);

        for (int i = 0; i < rowSize; i++)
        {
            for (int k = 0; k < colSize; k++)
            {
                float res = MAT_IDX(m, i, k) * s;
                assert(FLOAT_EQUAL(res, MAT_IDX(out, i, k)));
            }
        }


        Matrix_Copy(&m, &out);
        Matrix_Scale(&out, s, &out); //自身scale

        for (int i = 0; i < rowSize; i++)
        {
            for (int k = 0; k < colSize; k++)
            {
                float res = MAT_IDX(m, i, k) * s;
                assert(FLOAT_EQUAL(res, MAT_IDX(out, i, k)));
            }
        }

        free(m.data);
        free(out.data);
    }
}

void TransposeTest()
{
    for (int count = 0; count < 10; count++)
    {
        uint8_t rowSize, colSize;
        rowSize = (uint32_t)rand() % MAX_TEST_SIZE + 1;
        colSize = (uint32_t)rand() % MAX_TEST_SIZE + 1;

        Matrix m;
        m.row = rowSize;
        m.col = colSize;
        m.data = malloc(sizeof(float) * rowSize * colSize);

        Matrix out;
        out.row = m.col;
        out.col = m.row;
        out.data = malloc(sizeof(float) * rowSize * colSize);

        for (int i = 0; i < rowSize; i++)
        {
            for (int k = 0; k < colSize; k++)
            {
                float r = (float)rand();
                MAT_IDX(m, i, k) = r;
            }
        }

        Matrix_Transpose(&m, &out);

        for (int i = 0; i < rowSize; i++)
        {
            for (int k = 0; k < colSize; k++)
            {
                assert(FLOAT_EQUAL(MAT_IDX(m, i, k), MAT_IDX(out, k, i)));
            }
        }

        free(m.data);
        free(out.data);
    }
}

void SelfTransposeTest()
{
    for (int count = 0; count < 10; count++)
    {
        uint8_t size;
        size = (uint32_t)rand() % MAX_TEST_SIZE + 1;

        Matrix m;
        m.row = size;
        m.col = size;
        m.data = malloc(sizeof(float) * size * size);

        Matrix out;
        out.row = size;
        out.col = size;
        out.data = malloc(sizeof(float) * size * size);

        for (int i = 0; i < size; i++)
        {
            for (int k = 0; k < size; k++)
            {
                float r = (float)rand() / 20.0f;
                MAT_IDX(m, i, k) = r;
            }
        }

        Matrix_Copy(&m, &out);
        Matrix_SelfTranspose(&out);

        for (int i = 0; i < size; i++)
        {
            for (int k = 0; k < size; k++)
            {
                assert(FLOAT_EQUAL(MAT_IDX(m, i, k), MAT_IDX(out, k, i)));
            }
        }

        free(m.data);
        free(out.data);
    }
}


void MultiplyTest()
{
    for (int count = 0; count < 10; count++)
    {

        Matrix m1;
        m1.row = (uint32_t)rand() % MAX_TEST_SIZE + 1;
        m1.col = (uint32_t)rand() % MAX_TEST_SIZE + 1;
        m1.data = malloc(sizeof(float) * m1.row * m1.col);

        Matrix m2;
        m2.row = m1.col;
        m2.col = (uint32_t)rand() % MAX_TEST_SIZE + 1;
        m2.data = malloc(sizeof(float) * m2.row * m2.col);

        Matrix out;
        out.row = m1.row;
        out.col = m2.col;
        out.data = malloc(sizeof(float) * out.row * out.col);

        for (int i = 0; i < m1.row; i++)
        {
            for (int k = 0; k < m1.col; k++)
            {
                float r = (float)rand() / 20.0f;
                MAT_IDX(m1, i, k) = r;
            }
        }

        for (int i = 0; i < m2.row; i++)
        {
            for (int k = 0; k < m2.col; k++)
            {
                float r = (float)rand() / 20.0f;
                MAT_IDX(m2, i, k) = r;
            }
        }

        Matrix_Multiply(&m1, &m2, &out);

        for (int i = 0; i < out.row; i++)
        {
            for (int k = 0; k < out.col; k++)
            {
                float sum = 0;
                for(int x = 0; x < m1.col; x++)
                    sum += MAT_IDX(m1, i, x) * MAT_IDX(m2, x, k);

                assert(FLOAT_EQUAL(sum, MAT_IDX(out, i, k)));
            }
        }

        free(m1.data);
        free(m2.data);
        free(out.data);
    }
}

//1x1
float invIn1[1 * 1] = {10.0f};
float invRes1[1 * 1] = { 0.1f };

//2x2
float invIn2[2 * 2] = { 10.0f,    3.0f,
                          2.7f,     152.8f};

float invRes2[2 * 2] = { 0.100533f,     -0.001974f,
                         -0.001776f,    0.006579f };

float* invInputs[] = {invIn1 , invIn2};
float* invResults[] = { invRes1, invRes2};

void InverseTest()
{
    char buff[512];
    for (int i = 0; i < sizeof(invInputs) / sizeof(float*); i++)
    {
        uint8_t size = i + 1;

        Matrix m;
        m.row = size;
        m.col = size;
        m.data = invInputs[i];

        Matrix inv;
        inv.row = size;
        inv.col = size;
        inv.data = malloc(sizeof(float) * size * size);

        Matrix mul;
        mul.row = size;
        mul.col = size;
        mul.data = malloc(sizeof(float) * size * size);

        Matrix_Inverse(&m, &inv);
        Matrix_Multiply(&m, &inv, &mul);

        Matrix_ToString(&inv, buff);
        printf("----------------------\n");
        printf(buff);
        printf("\n");

        free(inv.data);
        free(mul.data);
    }
}
