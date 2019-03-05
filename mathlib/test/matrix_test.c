#include "matrix.h"
#include "stdio.h"
#include "stdlib.h"
#include "assert.h"
#include "time.h"
#include "mathex.h"

#define MAX_TEST_SIZE 8

void IdentityTest(void)
{
    //正确数据测试
    for (int i = 1; i < MAX_TEST_SIZE; i++)
    {
        Matrix m;
        m.row = i;
        m.col = i;
        m.data = malloc(sizeof(MAT_FLOAT) * i * i);

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
            m.data = malloc(sizeof(MAT_FLOAT) * i * k);

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
        m1.data = malloc(sizeof(MAT_FLOAT) * rowSize * colSize);

        Matrix m2;
        m2.row = rowSize;
        m2.col = colSize;
        m2.data = malloc(sizeof(MAT_FLOAT) * rowSize * colSize);

        for (int i = 0; i < rowSize; i++)
        {
            for (int k = 0; k < colSize; k++)
            {
                MAT_FLOAT r = (MAT_FLOAT)rand() / 20.0f;
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
        m1.data = malloc(sizeof(MAT_FLOAT) * rowSize * colSize);

        Matrix m2;
        m2.row = rowSize;
        m2.col = colSize;
        m2.data = malloc(sizeof(MAT_FLOAT) * rowSize * colSize);

        for (int i = 0; i < rowSize; i++)
        {
            for (int k = 0; k < colSize; k++)
            {
                MAT_FLOAT r = (MAT_FLOAT)rand() / 20.0f;
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
        m1.data = malloc(sizeof(MAT_FLOAT) * rowSize * colSize);

        Matrix m2;
        m2.row = rowSize;
        m2.col = colSize;
        m2.data = malloc(sizeof(MAT_FLOAT) * rowSize * colSize);

        Matrix out;
        out.row = rowSize;
        out.col = colSize;
        out.data = malloc(sizeof(MAT_FLOAT) * rowSize * colSize);

        for (int i = 0; i < rowSize; i++)
        {
            for (int k = 0; k < colSize; k++)
            {
                MAT_FLOAT r = (MAT_FLOAT)rand() / 20.0f;
                MAT_IDX(m1, i, k) = r;
                r = (MAT_FLOAT)rand() / 20.0f;
                MAT_IDX(m2, i, k) = r;
            }
        }

        Matrix_Add(&m1, &m2, &out);

        for (int i = 0; i < rowSize; i++)
        {
            for (int k = 0; k < colSize; k++)
            {
                MAT_FLOAT sum = MAT_IDX(m1, i, k) + MAT_IDX(m2, i, k);
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
        m1.data = malloc(sizeof(MAT_FLOAT) * rowSize * colSize);

        Matrix m2;
        m2.row = rowSize;
        m2.col = colSize;
        m2.data = malloc(sizeof(MAT_FLOAT) * rowSize * colSize);

        Matrix out;
        out.row = rowSize;
        out.col = colSize;
        out.data = malloc(sizeof(MAT_FLOAT) * rowSize * colSize);

        for (int i = 0; i < rowSize; i++)
        {
            for (int k = 0; k < colSize; k++)
            {
                MAT_FLOAT r = (MAT_FLOAT)rand() / 20.0f;
                MAT_IDX(m1, i, k) = r;
                r = (MAT_FLOAT)rand() / 20.0f;
                MAT_IDX(m2, i, k) = r;
            }
        }

        Matrix_Sub(&m1, &m2, &out);

        for (int i = 0; i < rowSize; i++)
        {
            for (int k = 0; k < colSize; k++)
            {
                MAT_FLOAT sum = MAT_IDX(m1, i, k) - MAT_IDX(m2, i, k);
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
        m.data = malloc(sizeof(MAT_FLOAT) * rowSize * colSize);

        Matrix out;
        out.row = rowSize;
        out.col = colSize;
        out.data = malloc(sizeof(MAT_FLOAT) * rowSize * colSize);

        for (int i = 0; i < rowSize; i++)
        {
            for (int k = 0; k < colSize; k++)
            {
                MAT_FLOAT r = (MAT_FLOAT)rand() / 20.0f;
                MAT_IDX(m, i, k) = r;
            }
        }

        MAT_FLOAT s = (MAT_FLOAT)rand() / 20.0f;
        Matrix_Scale(&m, s, &out);

        for (int i = 0; i < rowSize; i++)
        {
            for (int k = 0; k < colSize; k++)
            {
                MAT_FLOAT res = MAT_IDX(m, i, k) * s;
                assert(FLOAT_EQUAL(res, MAT_IDX(out, i, k)));
            }
        }


        Matrix_Copy(&m, &out);
        Matrix_Scale(&out, s, &out); //自身scale

        for (int i = 0; i < rowSize; i++)
        {
            for (int k = 0; k < colSize; k++)
            {
                MAT_FLOAT res = MAT_IDX(m, i, k) * s;
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
        m.data = malloc(sizeof(MAT_FLOAT) * rowSize * colSize);

        Matrix out;
        out.row = m.col;
        out.col = m.row;
        out.data = malloc(sizeof(MAT_FLOAT) * rowSize * colSize);

        for (int i = 0; i < rowSize; i++)
        {
            for (int k = 0; k < colSize; k++)
            {
                MAT_FLOAT r = (MAT_FLOAT)rand();
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
        m.data = malloc(sizeof(MAT_FLOAT) * size * size);

        Matrix out;
        out.row = size;
        out.col = size;
        out.data = malloc(sizeof(MAT_FLOAT) * size * size);

        for (int i = 0; i < size; i++)
        {
            for (int k = 0; k < size; k++)
            {
                MAT_FLOAT r = (MAT_FLOAT)rand() / 20.0f;
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
        m1.data = malloc(sizeof(MAT_FLOAT) * m1.row * m1.col);

        Matrix m2;
        m2.row = m1.col;
        m2.col = (uint32_t)rand() % MAX_TEST_SIZE + 1;
        m2.data = malloc(sizeof(MAT_FLOAT) * m2.row * m2.col);

        Matrix out;
        out.row = m1.row;
        out.col = m2.col;
        out.data = malloc(sizeof(MAT_FLOAT) * out.row * out.col);

        for (int i = 0; i < m1.row; i++)
        {
            for (int k = 0; k < m1.col; k++)
            {
                MAT_FLOAT r = (MAT_FLOAT)rand() / 20.0f;
                MAT_IDX(m1, i, k) = r;
            }
        }

        for (int i = 0; i < m2.row; i++)
        {
            for (int k = 0; k < m2.col; k++)
            {
                MAT_FLOAT r = (MAT_FLOAT)rand() / 20.0f;
                MAT_IDX(m2, i, k) = r;
            }
        }

        Matrix_Multiply(&m1, &m2, &out);

        for (int i = 0; i < out.row; i++)
        {
            for (int k = 0; k < out.col; k++)
            {
                MAT_FLOAT sum = 0;
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
MAT_FLOAT invIn1[1 * 1] = { 10.0f };
MAT_FLOAT invRes1[1 * 1] = { 0.1f };

//2x2
MAT_FLOAT invIn2[2 * 2] = { 10.0f,    3.0f,
                          2.7f,     152.8f };

MAT_FLOAT invRes2[2 * 2] = { 0.100533f,     -0.001974f,
                         -0.001776f,    0.006579f };

//3x3
MAT_FLOAT invIn3[3 * 3] = { 7.020f,  4.220f,  2.260f,
                        4.500f,  5.700f,  4.000f,
                        0.380f,  6.900f,  7.980f, };
MAT_FLOAT invRes3[3 * 3] = { 0.391278f, -0.395557f, 0.087461f,
                        -0.752323f,	1.206709f, -0.391803f,
                        0.631872f, -1.024559f, 0.459925f,
};

//4x4
MAT_FLOAT invIn4[4 * 4] = { 6.920f, 2.780f, 3.520f, 1.240f,
                        5.100f, 7.180f, 2.680f, 1.160f,
                        3.080f, 5.920f, 9.580f, 1.140f,
                        1.580f, 5.900f, 8.980f, 7.600f, };
MAT_FLOAT invRes4[4 * 4] = { 0.181764f, -0.018602f, -0.042407f, -0.020456f,
                        -0.146198f, 0.197487f, 0.005081f, -0.007051f,
                        0.026643f, -0.114352f, 0.132999f, -0.006843f,
                        0.044227f, -0.014329f, -0.152277f,	0.149392f,
};

//5x5
MAT_FLOAT invIn5[5 * 5] = { 47.700f, 4.500f, 2.500f, 2.200f, 37.200f,
                        45.600f, 40.300f, 13.600f, 15.600f, 19.800f,
                        17.800f, 6.000f, 31.200f, 43.800f, 35.200f,
                        3.300f, 28.900f, 14.200f, 16.700f, 15.200f,
                        8.500f, 3.000f, 32.200f, 10.200f, 30.200f, };
MAT_FLOAT invRes5[5 * 5] = { -0.004238f, 0.027026f, 0.004902f, -0.038147f, 0.000987f,
                        -0.000290f, 0.003618f, - 0.012934f, 0.032637f, -0.003366f,
                        -0.029809f, 0.027570f, - 0.003841f, -0.037358f, 0.041922f,
                        -0.004843f, -0.001498f, 0.030377f, -0.000537f, -0.028188f,
                        0.034640f, -0.036856f, - 0.006259f, 0.047508f, -0.002009f, };

////6x6
MAT_FLOAT invIn6[6 * 6] = { 32.100f, 24.400f, 39.500f, 26.100f, 24.600f, 36.900f,
                        46.200f, 9.400f, 6.900f, 8.200f, 48.200f, 41.400f,
                        6.800f, 47.100f, 40.300f, 18.200f, 14.000f, 36.300f,
                        11.400f, 22.500f, 13.200f, 0.400f, 24.100f, 30.100f,
                        5.200f, 29.200f, 47.000f, 3.400f, 24.900f, 35.100f,
                        40.500f, 47.200f, 34.500f, 48.500f, 1.200f, 16.600f, };
MAT_FLOAT invRes6[6 * 6] = { 0.181665f, -0.240522f, -0.357258f, 0.506949f, -0.007212f, 0.073292f,
                        - 0.052962f, 0.004921f, -0.004170f, 0.024332f, 0.020261f, 0.027613f,
                        0.036877f, -0.050843f, -0.084604f, 0.074433f, 0.036678f, 0.017314f,
                        - 0.191336f, 0.289706f, 0.415114f, -0.610156f, -0.010978f, -0.075369f,
                        - 0.31934f, 0.37591f, 0.444176f, -0.718683f, 0.076630f, -0.057830f,
                        0.212841f, -0.195115f, -0.185626f, 0.373924f, -0.089708f, -0.008688f,};

//7x7
MAT_FLOAT invIn7[7 * 7] = { 3.892f, 1.243f, 1.892f, 5.270f, 11.216f, 8.973f, 7.892f,
                        7.351f, 10.135f, 2.568f, 1.946f, 11.676f, 4.946f, 1.541f,
                        13.459f, 7.135f, 3.919f, 10.324f, 11.649f, 5.649f, 0.297f,
                        11.405f, 9.703f, 2.486f, 10.324f, 9.054f, 10.081f, 3.486f,
                        1.811f, 7.432f, 0.514f, 5.324f, 4.946f, 7.730f, 4.486f,
                        5.919f, 7.108f, 9.216f, 5.946f, 7.270f, 0.514f, 6.946f,
                        0.324f, 10.324f, 13.162f, 8.135f, 4.514f, 2.649f, 8.649f, };
MAT_FLOAT invRes7[7 * 7] = { -0.005642f, 0.012463f, -0.139319f, 0.232941f, -0.189558f, 0.086255f, -0.057129f,
                        - 0.090885f, 0.034352f, -0.003841f, -0.042878f, 0.150089f, 0.064467f, -0.035396f,
                        0.066735f, 0.078445f, -0.089626f, 0.181956f, -0.352139f, -0.190809f, 0.190753f,
                        - 0.046115f, -0.144609f, 0.232017f, -0.234444f, 0.290847f, 0.046963f, -0.034200f,
                        0.036656f, 0.031210f, 0.150985f, -0.226976f, 0.128202f, -0.007331f, -0.013317f,
                        0.090433f, 0.075805f, -0.153944f, 0.268277f, -0.295546f, -0.238032f, 0.145587f,
                        0.003687f, -0.064341f, -0.103683f, 0.022360f, 0.113876f, 0.242748f, -0.135747f,
                        };

////8x8
MAT_FLOAT invIn8[8 * 8] = { 3.595f, 10.162f, 12.000f, 3.784f, 6.297f, 1.541f, 9.703f, 12.865f,
                        11.946f, 9.081f, 9.081f, 2.486f, 6.892f, 7.622f, 0.189f, 7.459f,
                        13.027f, 4.730f, 11.351f, 6.000f, 9.514f, 2.216f, 2.486f, 9.108f,
                        6.216f, 2.378f, 0.216f, 4.973f, 4.811f, 2.459f, 4.135f, 8.027f,
                        4.243f, 5.270f, 9.216f, 12.000f, 10.216f, 2.730f, 2.189f, 2.865f,
                        2.568f, 12.811f, 7.216f, 0.189f, 6.568f, 5.135f, 0.135f, 6.703f,
                        8.649f, 6.405f, 4.838f, 9.324f, 7.919f, 12.054f, 11.514f, 9.243f,
                        12.486f, 5.892f, 8.459f, 3.108f, 10.676f, 6.784f, 12.135f, 4.676f, };
MAT_FLOAT invRes8[8 * 8] = { 0.075270f, 0.304988f, -0.25253f, 0.166923f, 0.083063f, -0.19914f, -0.166206f, 0.074852f,
                        0.148197f, 0.395835f, -0.464801f, 0.243738f, 0.172823f, -0.184043f, -0.24581f, 0.091608f,
                        0.010910f, -0.076780f, 0.159801f, -0.200774f, -0.039171f, 0.018596f, 0.085798f, -0.046399f,
                        0.091907f, 0.240478f, -0.254662f, 0.133683f, 0.156022f, -0.187704f, -0.097062f, -0.004576f,
                        - 0.198211f, -0.447954f, 0.394505f, -0.112238f, -0.114748f, 0.323421f, 0.146749f, 0.000751f,
                        - 0.117073f, -0.198886f, 0.247002f, -0.208413f, -0.11715f, 0.152313f, 0.226576f, -0.078421f,
                        0.076254f, 0.071221f, -0.141107f, 0.049674f, 0.031735f, -0.090691f, -0.049478f, 0.074537f,
                        - 0.044043f, -0.207643f, 0.247301f, -0.048782f, -0.122814f, 0.130739f, 0.127477f, -0.095843f,
                        };

MAT_FLOAT* invInputs[] = { invIn1, invIn2, invIn3, invIn4, invIn5, invIn6, invIn7, invIn8 };
MAT_FLOAT* invResults[] = { invRes1, invRes2, invRes3, invRes4, invRes5, invRes6, invRes7, invRes8 };

void InverseTest()
{
    for (int i = 0; i < sizeof(invInputs) / sizeof(MAT_FLOAT*); i++)
    {
        uint8_t size = i + 1;

        Matrix m;
        m.row = size;
        m.col = size;
        m.data = invInputs[i];

        Matrix inv;
        inv.row = size;
        inv.col = size;
        inv.data = malloc(sizeof(MAT_FLOAT) * size * size);

        Matrix_Inverse(&m, &inv);

        Matrix result;
        result.row = size;
        result.col = size;
        result.data = invResults[i];

        assert(Matrix_Equal(&inv, &result, 0.0001f));
        
        free(inv.data);
    }
}

//1
MAT_FLOAT eqs1[2] = { 13.444f,	184.737f };
MAT_FLOAT sol1[1] = { 13.7412f };

//2
MAT_FLOAT eqs2[6] = { 14.000f, 0.222f, 140.412f,
                    17.222f, 17.407f, 484.314f,};
MAT_FLOAT sol2[2] = { 9.7411f,
                    18.1854f };

//3
MAT_FLOAT eqs3[12] = { 11.296f, 6.519f, 0.407f, 156.789f,
                    16.778f, 4.889f, 9.630f, 299.680f,
                    16.926f, 8.963f, 0.407f, 231.126f,};
MAT_FLOAT sol3[3] = { 12.1070f,
                    2.5265f,
                    8.7432f};

//4
MAT_FLOAT eqs4[20] = { 13.593f, 3.630f, 6.889f, 17.667f, 541.641f,
                    14.074f, 16.667f, 0.037f, 16.185f, 527.753f,
                    11.074f, 11.444f, 2.741f, 15.481f, 485.641f,
                    14.593f, 6.259f, 12.444f, 2.000f, 405.879f,};
MAT_FLOAT sol4[4] = { 9.4024f,
                    8.2259f,
                    14.8932f,
                    15.9266f};

//5
MAT_FLOAT eqs5[30] = { 7.963f, 9.000f, 5.963f, 8.222f, 0.778f, 318.567f,
                    1.259f, 6.519f, 4.778f, 9.889f, 8.444f, 314.573f,
                    17.926f, 7.926f, 13.444f, 2.481f, 17.111f, 550.966f,
                    9.000f, 5.667f, 8.074f, 4.222f, 11.889f, 370.261f,
                    13.519f, 10.519f, 15.630f, 1.519f, 18.444f, 610.846f,};
MAT_FLOAT sol5[5] = { 3.3989f,
                    12.9699f,
                    17.5959f,
                    7.7686f,
                    7.6795f};

//6
MAT_FLOAT eqs6[42] = { 13.963f, 5.556f, 12.000f, 16.667f, 16.815f, 12.741f, 733.941f,
                    15.222f, 16.000f, 9.593f, 9.259f, 17.778f, 10.333f, 722.871f,
                    16.481f, 0.296f, 9.407f, 18.222f, 12.444f, 0.519f, 650.874f,
                    13.074f, 5.630f, 15.481f, 13.296f, 4.741f, 0.222f, 559.119f,
                    15.963f, 18.148f, 14.037f, 9.481f, 0.037f, 15.704f, 659.263f,
                    7.444f, 8.741f, 8.185f, 8.667f, 15.296f, 12.704f, 516.724f,};
MAT_FLOAT sol6[6] = { 17.5778f,
                    5.6443f,
                    8.4716f,
                    9.3917f,
                    8.5527f,
                    4.3276f};

//7
MAT_FLOAT eqs7[56] = { 1.444f, 4.222f, 1.481f, 0.333f, 11.963f, 12.000f, 1.815f, 172.948f,
                    18.259f, 12.148f, 4.963f, 6.000f, 7.222f, 10.185f, 11.148f, 336.621f,
                    16.444f, 11.778f, 11.296f, 10.407f, 4.296f, 2.444f, 11.852f, 363.539f,
                    8.667f, 15.444f, 8.815f, 14.667f, 6.333f, 5.037f, 5.074f, 405.055f,
                    7.037f, 5.593f, 12.111f, 11.963f, 4.667f, 11.185f, 15.444f, 306.609f,
                    1.407f, 8.519f, 16.074f, 9.222f, 15.889f, 9.556f, 11.630f, 421.693f,
                    14.259f, 7.519f, 17.667f, 5.444f, 0.630f, 4.407f, 13.926f, 294.728f,};
MAT_FLOAT sol7[7] = { 0.8171f,
                    13.2197f,
                    5.4446f,
                    5.0760f,
                    7.8901f,
                    0.4077f,
                    3.8121f};

//8
MAT_FLOAT eqs8[72] = { 18.037f, 6.889f, 13.778f, 9.444f, 5.778f, 16.000f, 4.444f, 8.704f, 980.376f,
                    9.593f, 8.074f, 7.815f, 5.407f, 14.333f, 0.333f, 16.852f, 4.185f, 750.040f,
                    15.667f, 3.037f, 4.296f, 5.667f, 11.852f, 13.963f, 11.519f, 12.556f, 993.798f,
                    2.222f, 1.333f, 13.741f, 6.778f, 5.037f, 2.778f, 1.148f, 10.222f, 453.697f,
                    18.407f, 15.222f, 12.556f, 3.222f, 8.815f, 16.778f, 14.556f, 11.148f, 1197.698f,
                    11.852f, 7.704f, 11.667f, 17.519f, 7.037f, 6.037f, 14.815f, 15.444f, 1036.104f,
                    14.778f, 0.889f, 7.815f, 17.481f, 15.074f, 7.815f, 6.000f, 6.000f, 851.490f,
                    4.481f, 16.852f, 12.222f, 1.741f, 11.926f, 2.593f, 4.074f, 0.630f, 476.219f,};
MAT_FLOAT sol8[8] = { 12.4851f,
                    2.6659f,
                    10.7794f,
                    7.8138f,
                    8.4432f,
                    18.2929f,
                    18.4809f,
                    10.4439f};

MAT_FLOAT* eqsInputs[8] = { eqs1, eqs2, eqs3, eqs4, eqs5, eqs6, eqs7, eqs8, };
MAT_FLOAT* solInputs[8] = { sol1, sol2, sol3, sol4, sol5, sol6, sol7, sol8, };
void EquationsTest()
{
    for (int i = 1; i <= MAX_TEST_SIZE; i++)
    {
        Matrix M;
        M.row = i;
        M.col = i + 1;
        M.data = eqsInputs[i - 1];


        MAT_FLOAT* sol = malloc(sizeof(MAT_FLOAT) * i);

        Matrix_RREF(&M, sol);

        Matrix x;
        x.row = i;
        x.col = 1;
        x.data = sol;
        //PrintMatrix(&x);

        for (int k = 0; k < i; k++)
        {
            assert(FLOAT_NEAR(sol[k], *(solInputs[i - 1] + k), 0.0001f));
        }

        free(sol);
    }
}

static char printBuff[1024];
void PrintMatrix(Matrix* m)
{
    printf("---------\n");
    Matrix_ToString(m, printBuff, " ");
    printf(printBuff);
    printf("---------\n");
}

void RandMatrix()
{
    for (int size = 1; size <= MAX_TEST_SIZE; size++)
    {
        Matrix m;
        m.row = size;
        m.col = size;
        m.data = malloc(sizeof(MAT_FLOAT) * m.row * m.col);


        for (int i = 0; i < m.row; i++)
        {
            for (int k = 0; k < m.col; k++)
            {
                MAT_FLOAT r = (MAT_FLOAT)(rand() % 500) / 37.0f;
                MAT_IDX(m, i, k) = r;
            }
        }

        
        PrintMatrix(&m);
    }
}

void RandEquations()
{
    srand((uint32_t)time(NULL));
    for (int size = 1; size <= MAX_TEST_SIZE; size++)
    {
        Matrix m;
        m.row = size;
        m.col = size + 1;
        m.data = malloc(sizeof(MAT_FLOAT) * m.row * m.col);

        Matrix A;
        A.row = size;
        A.col = size;
        A.data = malloc(sizeof(MAT_FLOAT) * A.row * A.col);

        Matrix x;
        x.row = size;
        x.col = 1;
        x.data = malloc(sizeof(MAT_FLOAT) * x.row * x.col);

        Matrix b;
        b.row = size;
        b.col = 1;
        b.data = malloc(sizeof(MAT_FLOAT) * b.row * b.col);

        for (int i = 0; i < m.row; i++)
        {
            for (int k = 0; k < m.col - 1; k++)
            {
                MAT_FLOAT r = (MAT_FLOAT)(rand() % 500) / 27.0f;
                MAT_IDX(m, i, k) = r;
                MAT_IDX(A, i, k) = r;
            }
        }


        for (int i = 0; i < x.row; i++)
        {
            MAT_FLOAT r = (MAT_FLOAT)(rand() % 500) / 27.0f;
            MAT_IDX(x, i, 0) = r;
        }

        Matrix_Multiply(&A, &x, &b);

        for (int i = 0; i < m.row; i++)
            MAT_IDX(m, i, m.col - 1) = MAT_IDX(b, i, 0);


        PrintMatrix(&m);
        PrintMatrix(&x);
        printf("*******************\n");

        free(m.data);
        free(A.data);
        free(x.data);
        free(b.data);
    }
}


/*IdentityTest();
EqualTest();
CopyTest();
AddTest();
SubTest();
ScaleTest();
TransposeTest();
SelfTransposeTest();
MultiplyTest();
InverseTest();
EquationsTest();*/
