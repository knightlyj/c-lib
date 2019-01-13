#include "matrix.h"
#include "stdio.h"
#include "string.h"

uint8_t Matrix_Identity(Matrix* m)
{
    if (m->row != m->col)
    {
        return 0;
    }

    uint8_t row, col;
    row = m->row;
    col = m->col;
    for (int i = 0; i < row; i++)
    {
        for (int k = 0; k < col; k++)
        {
            if (i == k)
            {
                MAT_IDX(*m, i, k) = 1.0f;
            }
            else
            {
                MAT_IDX(*m, i, k) = 0.0f;
            }
        }
    }

    return 1;
}

uint8_t Matrix_Copy(Matrix* src, Matrix* dst)
{
    if (dst->row != src->row || dst->col != src->col)
        return 0;

    uint8_t row, col;
    row = src->row;
    col = src->col;
    for (int i = 0; i < row; i++)
    {
        for (int k = 0; k < col; k++)
        {
            MAT_IDX((*dst), i, k) = MAT_IDX((*src), i, k);
        }
    }
}

uint8_t Matrix_Equal(Matrix* m1, Matrix* m2, float tolerance)
{
    if (m1->row != m2->row || m1->col != m2->col)
        return 0;

    uint8_t row, col;
    row = m1->row;
    col = m1->col;
    for (int i = 0; i < row; i++)
    {
        for (int k = 0; k < col; k++)
        {
            if (!FLOAT_NEAR(MAT_IDX(*m1, i, k), MAT_IDX(*m2, i, k), tolerance))
                return 0;
        }
    }

    return 1;
}

uint8_t Matrix_Add(Matrix* m1, Matrix* m2, Matrix* out)
{
    if (m2->row != m1->row || out->row != m1->row
        || m2->col != m1->col || out->col != m1->col)
        return 0;

    uint8_t row, col;
    row = m1->row;
    col = m1->col;
    for (int i = 0; i < row; i++)
    {
        for (int k = 0; k < col; k++)
        {
            MAT_IDX(*out, i, k) = MAT_IDX(*m1, i, k) + MAT_IDX(*m2, i, k);
        }
    }

    return 1;
}

uint8_t Matrix_Sub(Matrix* m1, Matrix* m2, Matrix* out)
{
    if (m2->row != m1->row || out->row != m1->row
        || m2->col != m1->col || out->col != m1->col)
        return 0;

    uint8_t row, col;
    row = m1->row;
    col = m1->col;
    for (int i = 0; i < row; i++)
    {
        for (int k = 0; k < col; k++)
        {
            MAT_IDX(*out, i, k) = MAT_IDX(*m1, i, k) - MAT_IDX(*m2, i, k);
        }
    }

    return 1;
}

void Matrix_Scale(Matrix* m, float scale, Matrix* out)
{
    uint8_t rowSize, colSize;
    rowSize = m->row;
    colSize = m->col;
    for (int i = 0; i < rowSize; i++)
    {
        for (int k = 0; k < colSize; k++)
        {
            MAT_IDX(*out, i, k) = MAT_IDX(*m, i, k) * scale;
        }
    }
}

uint8_t Matrix_Multiply(Matrix* m1, Matrix* m2, Matrix* out)
{
    if (out->row != m1->row || out->col != m2->col || m1->col != m2->row)
    {
        return 0;
    }

    uint8_t row, col;
    row = out->row;
    col = out->col;
    for (int i = 0; i < row; i++)
    {
        for (int k = 0; k < col; k++)
        {
            float sum = 0;
            for (int x = 0; x < m1->col; x++)
                sum += MAT_IDX(*m1, i, x) * MAT_IDX(*m2, x, k);

            MAT_IDX(*out, i, k) = sum;
        }
    }

    return 1;
}

uint8_t Matrix_Transpose(Matrix* m, Matrix* out)
{
    if (out->row != m->col || out->col != m->row)
        return 0;

    uint8_t row, col;
    row = out->row;
    col = out->col;
    for (int i = 0; i < row; i++)
    {
        for (int k = 0; k < col; k++)
        {
            MAT_IDX(*out, i, k) = MAT_IDX(*m, k, i);
        }
    }

    return 1;
}

uint8_t Matrix_SelfTranspose(Matrix* m)
{
    if (m->col != m->row)
        return 0;

    uint8_t size = m->row;

    for (int i = 0; i < size; i++)
    {
        for (int k = i + 1; k < size; k++)
        {
            float tmp = MAT_IDX(*m, i, k);
            MAT_IDX(*m, i, k) = MAT_IDX(*m, k, i);
            MAT_IDX(*m, k, i) = tmp;
        }
    }

    return 1;
}

//三种初等行变换
static void SwapRow(Matrix* m, uint8_t row1, uint8_t row2)
{
    float *pRow1, *pRow2;
    pRow1 = m->data + (int)row1 * m->col;
    pRow2 = m->data + (int)row2 * m->col;
    for (int i = 0; i < m->col; i++)
    {
        float tmp;
        tmp = pRow1[i];
        pRow1[i] = pRow2[i];
        pRow2[i] = tmp;
    }
}

static void ScaleRow(Matrix* m, uint8_t row, float scale)
{
    float* pRow = m->data + (int)row * m->col;
    for (int i = 0; i < m->col; i++)
    {
        pRow[i] *= scale;
    }
}

static void AddScaleRow(Matrix* m, uint8_t srcRow, float scale, uint8_t dstRow)
{
    float *pSrcRow, *pDstRow;
    pSrcRow = m->data + (int)srcRow * m->col;
    pDstRow = m->data + (int)dstRow * m->col;

    for (int i = 0; i < m->col; i++)
    {
        pDstRow[i] += pSrcRow[i] * scale;
    }
}

uint8_t Matrix_Inverse(Matrix* m, Matrix* out)
{
    if (m->col != m->row || out->row != m->row || out->col != m->col)
        return 0;

    uint8_t size = m->row;
    float* pRow;
    const float tolerance = 0.0003f;

    //out = I
    Matrix_Identity(out);

    
    //先处理成上三角矩阵
    for (int i = 0; i < size; i++)
    {
        pRow = m->data + i * size;

        if (FLOAT_NEAR(pRow[i], 0, tolerance))
        { //阶梯首元素为0,需要交换行
            int maxAbsRow = -1;
            float maxAbosute = 0;

            //找到当前列绝对值最大的一行, 交换
            for (int k = i + 1; k < size; k++)
            {
                float absolute = fabsf(m->data[k * m->col + i]);
                if (absolute > maxAbosute)
                {
                    maxAbsRow = k;
                    maxAbosute = absolute;
                }
            }

            if (maxAbsRow < 0)
            {//singular
                return 0;
            }
            else
            {
                SwapRow(m, i, maxAbsRow); //可优化
                SwapRow(out, i, maxAbsRow);
            }
        }

        //阶梯首元素归1
        float scale = 1 / pRow[i];
        ScaleRow(m, i, scale); //可优化
        ScaleRow(out, i, scale);

        for (int k = i + 1; k < size; k++)
        {
            scale = -m->data[k * m->col + i];
            AddScaleRow(m, i, scale, k);  //可优化
            AddScaleRow(out, i, scale, k);
        }

        
    }
    
    //再消掉右上角的部分
    for (int i = size - 1; i >= 0; i--)
    {
        for (int k = i - 1; k >= 0; k--)
        {
            float scale = -m->data[k * m->col + i];
            AddScaleRow(m, i, scale, k); //可优化
            AddScaleRow(out, i, scale, k);
        }
    }

    return 1;
}

char* Matrix_ToString(Matrix* m, char* buff, char* seperator)
{
    buff[0] = '\0';
    for (int i = 0; i < m->row; i++) 
    {
        for (int k = 0; k < m->col; k++)
        {
            int len = strlen(buff);
            sprintf(buff + len, "%.3f%s", MAT_IDX(*m, i, k), seperator);
        }
        strcat(buff, "\n");
    }

    return buff;
}
