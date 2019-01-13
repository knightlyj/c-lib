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
                for (int x = 0; x < m1.col; x++)
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
float invIn1[1 * 1] = { 10.0f };
float invRes1[1 * 1] = { 0.1f };

//2x2
float invIn2[2 * 2] = { 10.0f,    3.0f,
                          2.7f,     152.8f };

float invRes2[2 * 2] = { 0.100533f,     -0.001974f,
                         -0.001776f,    0.006579f };

//3x3
float invIn3[3 * 3] = { 7.020,  4.220,  2.260,
                        4.500,  5.700,  4.000,
                        0.380,  6.900,  7.980, };
float invRes3[3 * 3] = { 0.391278f, -0.395557f, 0.087461f,
                        -0.752323f,	1.206709f, -0.391803f,
                        0.631872f, -1.024559, 0.459925f,
};

//4x4
float invIn4[4 * 4] = { 6.920, 2.780, 3.520, 1.240,
                        5.100, 7.180, 2.680, 1.160,
                        3.080, 5.920, 9.580, 1.140,
                        1.580, 5.900, 8.980, 7.600, };
float invRes4[4 * 4] = { 0.181764, -0.018602, -0.042407, -0.020456,
                        -0.146198, 0.197487, 0.005081, -0.007051,
                        0.026643, -0.114352, 0.132999, -0.006843,
                        0.044227, -0.014329, -0.152277,	0.149392,
};

//5x5
float invIn5[5 * 5] = { 47.700, 4.500, 2.500, 2.200, 37.200,
                        45.600, 40.300, 13.600, 15.600, 19.800,
                        17.800, 6.000, 31.200, 43.800, 35.200,
                        3.300, 28.900, 14.200, 16.700, 15.200,
                        8.500, 3.000, 32.200, 10.200, 30.200, };
float invRes5[5 * 5] = { -0.004238, 0.027026, 0.004902, -0.038147, 0.000987,
                        -0.000290, 0.003618, - 0.012934, 0.032637, -0.003366,
                        -0.029809, 0.027570, - 0.003841, -0.037358, 0.041922,
                        -0.004843, -0.001498, 0.030377, -0.000537, -0.028188,
                        0.034640, -0.036856, - 0.006259, 0.047508, -0.002009, };

////6x6
float invIn6[6 * 6] = { 32.100, 24.400, 39.500, 26.100, 24.600, 36.900,
                        46.200, 9.400, 6.900, 8.200, 48.200, 41.400,
                        6.800, 47.100, 40.300, 18.200, 14.000, 36.300,
                        11.400, 22.500, 13.200, 0.400, 24.100, 30.100,
                        5.200, 29.200, 47.000, 3.400, 24.900, 35.100,
                        40.500, 47.200, 34.500, 48.500, 1.200, 16.600, };
float invRes6[6 * 6] = { 0.181665, -0.240522, -0.357258, 0.506949, -0.007212, 0.073292,
                        - 0.052962, 0.004921, -0.004170, 0.024332, 0.020261, 0.027613,
                        0.036877, -0.050843, -0.084604, 0.074433, 0.036678, 0.017314,
                        - 0.191336, 0.289706, 0.415114, -0.610156, -0.010978, -0.075369,
                        - 0.31934, 0.37591, 0.444176, -0.718683, 0.076630, -0.057830,
                        0.212841, -0.195115, -0.185626, 0.373924, -0.089708, -0.008688,};

//7x7
float invIn7[7 * 7] = { 3.892, 1.243, 1.892, 5.270, 11.216, 8.973, 7.892,
                        7.351, 10.135, 2.568, 1.946, 11.676, 4.946, 1.541,
                        13.459, 7.135, 3.919, 10.324, 11.649, 5.649, 0.297,
                        11.405, 9.703, 2.486, 10.324, 9.054, 10.081, 3.486,
                        1.811, 7.432, 0.514, 5.324, 4.946, 7.730, 4.486,
                        5.919, 7.108, 9.216, 5.946, 7.270, 0.514, 6.946,
                        0.324, 10.324, 13.162, 8.135, 4.514, 2.649, 8.649, };
float invRes7[7 * 7] = { -0.005642, 0.012463, -0.139319, 0.232941, -0.189558, 0.086255, -0.057129,
                        - 0.090885, 0.034352, -0.003841, -0.042878, 0.150089, 0.064467, -0.035396,
                        0.066735, 0.078445, -0.089626, 0.181956, -0.352139, -0.190809, 0.190753,
                        - 0.046115, -0.144609, 0.232017, -0.234444, 0.290847, 0.046963, -0.034200,
                        0.036656, 0.031210, 0.150985, -0.226976, 0.128202, -0.007331, -0.013317,
                        0.090433, 0.075805, -0.153944, 0.268277, -0.295546, -0.238032, 0.145587,
                        0.003687, -0.064341, -0.103683, 0.022360, 0.113876, 0.242748, -0.135747,
                        };

////8x8
float invIn8[8 * 8] = { 3.595, 10.162, 12.000, 3.784, 6.297, 1.541, 9.703, 12.865,
                        11.946, 9.081, 9.081, 2.486, 6.892, 7.622, 0.189, 7.459,
                        13.027, 4.730, 11.351, 6.000, 9.514, 2.216, 2.486, 9.108,
                        6.216, 2.378, 0.216, 4.973, 4.811, 2.459, 4.135, 8.027,
                        4.243, 5.270, 9.216, 12.000, 10.216, 2.730, 2.189, 2.865,
                        2.568, 12.811, 7.216, 0.189, 6.568, 5.135, 0.135, 6.703,
                        8.649, 6.405, 4.838, 9.324, 7.919, 12.054, 11.514, 9.243,
                        12.486, 5.892, 8.459, 3.108, 10.676, 6.784, 12.135, 4.676, };
float invRes8[8 * 8] = { 0.075270, 0.304988, -0.25253, 0.166923, 0.083063, -0.19914, -0.166206, 0.074852,
                        0.148197, 0.395835, -0.464801, 0.243738, 0.172823, -0.184043, -0.24581, 0.091608,
                        0.010910, -0.076780, 0.159801, -0.200774, -0.039171, 0.018596, 0.085798, -0.046399,
                        0.091907, 0.240478, -0.254662, 0.133683, 0.156022, -0.187704, -0.097062, -0.004576,
                        - 0.198211, -0.447954, 0.394505, -0.112238, -0.114748, 0.323421, 0.146749, 0.000751,
                        - 0.117073, -0.198886, 0.247002, -0.208413, -0.11715, 0.152313, 0.226576, -0.078421,
                        0.076254, 0.071221, -0.141107, 0.049674, 0.031735, -0.090691, -0.049478, 0.074537,
                        - 0.044043, -0.207643, 0.247301, -0.048782, -0.122814, 0.130739, 0.127477, -0.095843,
                        };

float* invInputs[] = { invIn1, invIn2, invIn3, invIn4, invIn5, invIn6, invIn7, invIn8 };
float* invResults[] = { invRes1, invRes2, invRes3, invRes4, invRes5, invRes6, invRes7, invRes8 };

void InverseTest()
{
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

        Matrix_Inverse(&m, &inv);

        Matrix result;
        result.row = size;
        result.col = size;
        result.data = invResults[i];

        assert(Matrix_Equal(&inv, &result, 0.0001f));
        
        free(inv.data);
    }
}

float buff[1024];
void RandMatrix()
{
    for (int size = 1; size <= MAX_TEST_SIZE; size++)
    {
        Matrix m;
        m.row = size;
        m.col = size;
        m.data = malloc(sizeof(float) * m.row * m.col);


        for (int i = 0; i < m.row; i++)
        {
            for (int k = 0; k < m.col; k++)
            {
                float r = (float)(rand() % 500) / 37.0f;
                MAT_IDX(m, i, k) = r;
            }
        }

        
        printf("-------------\n");
        Matrix_ToString(&m, buff, " ");
        printf(buff);
        printf("\n");
        printf("\n");
        Matrix_ToString(&m, buff, ", ");
        printf(buff);
    }
}
