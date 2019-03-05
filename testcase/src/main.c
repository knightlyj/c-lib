#include "stdio.h"
#include "stdint.h"
#include "matrix.h"
#include "vector3.h"

#define METHOD 1

#define SAMPLE_SIZE 6

Vector3 samples[] = {
    { 9.50, -1.09, 0.29 },
    { -9.92, -1.06, -0.15 },
    { -0.16, 8.71, -0.09 },
    { -0.32, -10.74, 0.96 },
    { -0.42, -1.15, 9.85 },
    { -1.08, -0.72, -9.85 },
    { 6.65, -4.76, -6.02 },
};

#if METHOD == 1

void PrintMatrix(Matrix* m);

#define MAX(a, b) (a>b ? a : b)

int main()
{
    float sx, sy, sz, ox, oy, oz;
    sx = sy = sz = 1.0f;
    ox = oy = oz = 0.0f;

    float gravity = 9.8f;

    for (int k = 0; k < 100000; k++)
    {
        float dSX = 0, dSY = 0, dSZ = 0;
        float dOX = 0, dOY = 0, dOZ = 0;
        for (int i = 0; i < SAMPLE_SIZE; i++)
        {
            float xi, yi, zi;
            xi = samples[i].x;
            yi = samples[i].y;
            zi = samples[i].z;
            float a, b, c;
            a = sx*xi + ox;
            b = sy*yi + oy;
            c = sz*zi + oz;
            float ui;
            ui = a*a + b*b + c*c - gravity*gravity;

            dSX += ui*a*xi;
            dOX += ui*a;
            dSY += ui*b*yi;
            dOY += ui*b;
            dSZ += ui*c*zi;
            dOZ += ui*c;
        }

        float max = MAX(fabs(dSX), fabs(dOX));
        max = MAX(max, fabs(dSY));
        max = MAX(max, fabs(dOY));
        max = MAX(max, fabs(dSZ));
        max = MAX(max, fabs(dOZ));

        float maxStep = 0.0001;
        if (max > maxStep)
        {
            float scale = maxStep / max;
            dSX *= scale;
            dOX *= scale;
            dSY *= scale;
            dOY *= scale;
            dSZ *= scale;
            dOZ *= scale;
        }


        float step = 1;
        sx -= step * dSX;
        ox -= step * dOX;

        sy -= step * dSY;
        oy -= step * dOY;

        sz -= step * dSZ;
        oz -= step * dOZ;


    }
    printf("x: %.5f, %.5f\n", sx, ox);
    printf("y: %.5f, %.5f\n", sy, oy);
    printf("z: %.5f, %.5f\n", sz, oz);
    printf("------------------\n");
    printf("********************\n");
    float error = 0;
    for (int i = 0; i < SAMPLE_SIZE; i++)
    {
        float xi, yi, zi;
        xi = samples[i].x;
        yi = samples[i].y;
        zi = samples[i].z;
        float sqrMag = xi*xi + yi*yi + zi*zi;
        printf("%.3f, ", sqrMag);

        float cx, cy, cz;
        cx = (xi + ox) * sx;
        cy = (yi + oy) * sy;
        cz = (zi + oz) * sz;
        sqrMag = cx*cx + cy*cy + cz*cz;
        printf("%.3f\n", sqrMag);
        float diff = sqrMag - 96.04;
        error += diff * diff;
    }
    printf("error %.3f\n", error);
    system("PAUSE");
    return 0;
}
#endif

#if 0

MAT_DEF(A, 7, 8);

void InputData(MAT_FLOAT x, MAT_FLOAT y, MAT_FLOAT z)
{
    MAT_FLOAT u[8];
    u[0] = x * x;
    u[1] = y * y;
    u[2] = z * z;
    u[3] = x;
    u[4] = y;
    u[5] = z;
    u[6] = 1;
    u[7] = 0;

    for (int i = 0; i < 7; i++)
    {
        for (int k = 0; k < 8; k++)
        {
            MAT_IDX(A, i, k) += u[i] * u[k];
        }
    }
}

int main(int argc, char* argv[])
{
    for (int i = 0; i < A.row; i++)
    {
        for (int k = 0; k < A.col; k++)
        {
            MAT_IDX(A, i, k) = 0;
        }
    }

    for (int i = 0; i < SAMPLE_SIZE; i++)
    {
        InputData(samples[i].x, samples[i].y, samples[i].z);
    }
    PrintMatrix(&A);

    MAT_FLOAT sol[7];
    SolveEquations(&A, sol);

    PrintMatrix(&A);

    system("PAUSE");
    return 0;
}
#endif

#if METHOD == 2
//------------------method2----------------------------

#define MATRIX_SIZE 7
#define u8 unsigned char

double m_matrix[MATRIX_SIZE][MATRIX_SIZE + 1];
int m = MATRIX_SIZE;
int n = MATRIX_SIZE + 1;
double m_result[MATRIX_SIZE];

void DispMatrix(void);

double Abs(double a)
{
    return a<0 ? -a : a;
}

u8 Equal(double a, double b)
{
    return Abs(a - b) < 1e-6;
}

void ResetMatrix(void)
{
    int row, column;

    for (row = 0; row<m; row++) {
        for (column = 0; column<n; column++)
            m_matrix[row][column] = 0.0f;
    }
}

void CalcData_Input(double x, double y, double z)
{
    double V[MATRIX_SIZE];
    int row, column;

    V[0] = x*x;
    V[1] = y*y;
    V[2] = z*z;
    V[3] = x;
    V[4] = y;
    V[5] = z;
    V[6] = 1.0;

    //构建VxVt矩阵(Vt为V的转置)，并进行累加
    for (row = 0; row<MATRIX_SIZE; row++) {
        for (column = 0; column<MATRIX_SIZE; column++) {
            m_matrix[row][column] += V[row] * V[column];
        }
    }
}

void SwapRow(int row1, int row2)
{
    int column;
    double tmp;

    for (column = 0; column<n; column++) {
        tmp = m_matrix[row1][column];
        m_matrix[row1][column] = m_matrix[row2][column];
        m_matrix[row2][column] = tmp;
    }
}

void MoveBiggestElement2Top(int s_row, int s_column)
{
    int row, column;

    for (row = s_row + 1; row<m; row++) {
        if (Abs(m_matrix[s_row][s_column])<Abs(m_matrix[row][s_column])) {
            SwapRow(s_row, row);
        }
    }
}

//高斯消元法，求行阶梯型矩阵
u8 Matrix_GaussElimination(void)
{
    int row, column, i, j;
    double tmp;

    for (row = 0, column = 0; row<m - 1 && column<n - 1; row++, column++) {
        //将当前列最大的一行移上来
        MoveBiggestElement2Top(row, column);

        //整列都为0
        if (Equal(m_matrix[row][column], 0.0f)) {
            printf("qiyi matrix:%d %d\r\n", row, column);
            //DispMatrix();
            //return 0;
            row--;
            continue;
        }

        //高斯消元
        for (i = row + 1; i<m; i++) {
            if (Equal(m_matrix[i][column], 0.0f))
                continue;	//为0，无需处理

            tmp = m_matrix[i][column] / m_matrix[row][column];

            for (j = column; j<n; j++) {
                m_matrix[i][j] -= m_matrix[row][j] * tmp;
            }
        }

        DispMatrix();
        printf("\r\n");
    }

    return 1;
}

//求行最简型矩阵
int Matrix_RowSimplify(void)
{
    int c = n;//返回值，表示(解的任意常量数+1)；
              //
    int row, column, k, s, t;
    double tmp;
    //
    for (row = 0, column = 0; row<m && column<n; row++, column++)
    {
        if (Equal(m_matrix[row][column], 0))//平移，找出本行第一个非零；
        {
            row--;
            continue;
        }
        //
        c--;//少一个常量；
            //
            //化a[i][j]为1；
        tmp = 1 / m_matrix[row][column];
        for (k = column; k<n; k++)//前面的"0"就不处理了；
            m_matrix[row][k] *= tmp;
        //
        //化a[s][j]为0
        for (s = 0; s<row; s++)//下面的0也不用处理；
        {
            if (Equal(m_matrix[s][column], 0))
                continue;//已经为0；
                         //
            tmp = m_matrix[s][column] / m_matrix[row][column];
            for (t = column; t<n; t++)
                m_matrix[s][t] -= m_matrix[row][t] * tmp;
            //
        }
    }
    //
    return c;
}

void Matrix_Solve(double* C, double* sol)
{
    int row, column, i;
    int any_sol[MATRIX_SIZE];

    //找出任意解的位置
    memset(any_sol, 0, MATRIX_SIZE);
    for (row = 0, column = 0; row<m && column<n - 1; row++, column++) {
        if (Equal(m_matrix[row][column], 0.0f)) {
            any_sol[column] = 1;	//记录任意解的位置
            row--;	//右移1列
        }
    }

    //求解
    row = 0;
    for (column = 0; column<n - 1; column++) {
        if (any_sol[column] == 1) {	//任意解
            sol[column] = C[column];
        }
        else {
            sol[column] = m_matrix[row][n - 1];
            //加上任意解
            for (i = column + 1; i<n - 1; i++) {
                if (any_sol[i] == 1 && !Equal(m_matrix[row][i], 0.0f)) {
                    sol[column] -= m_matrix[row][i] * C[i];
                }
            }
            row++;
        }
    }
}

void DispMatrix(void)
{
    int row, column;

    for (row = 0; row<m; row++) {
        for (column = 0; column<n; column++) {
            printf("%.3f,", m_matrix[row][column]);
        }
        printf("\r\n");
    }
    printf("-----------------------\n");
}

void Calc_Process(double radius)
{
    double C[MATRIX_SIZE];
    double Res[MATRIX_SIZE];
    int i;
    double k;

    ResetMatrix();

    //输入任意个数磁场测量点坐标，请尽量保证在椭球上分布均匀
    CalcData_Input(9.50, -1.09, 0.29);
    CalcData_Input(-9.92, -1.06, -0.15);
    CalcData_Input(-0.16, 8.71, -0.09);
    CalcData_Input(-0.32, -10.74, 0.96);
    //CalcData_Input(-0.42, -1.15, 9.85);
    //CalcData_Input(-1.08, -0.72, -9.85);
    DispMatrix();
    Matrix_GaussElimination();
    Matrix_RowSimplify();

    //赋值任意解参数值
    for (i = 0; i<MATRIX_SIZE; i++) {
        C[i] = 1.0f;
    }

    Matrix_Solve(C, Res);

    printf("a:%.2f b:%.2f c:%.2f d:%.2f e:%.2f f:%.2f g:%.2f\r\n", Res[0], Res[1], Res[2], Res[3], Res[4], Res[5], Res[6]);
    //d*d/a + 
    k = (Res[3] * Res[3] / Res[0] + Res[4] * Res[4] / Res[1] + Res[5] * Res[5] / Res[2] - 4 * Res[6]) / (4 * radius*radius);

    m_result[0] = sqrt(Res[0] / k);
    m_result[1] = sqrt(Res[1] / k);
    m_result[2] = sqrt(Res[2] / k);
    m_result[3] = Res[3] / (2 * Res[0]);
    m_result[4] = Res[4] / (2 * Res[1]);
    m_result[5] = Res[5] / (2 * Res[2]);

    printf("Xo:%f Yo:%f Zo:%f Xg:%f Yg:%f Zg:%f k:%f\r\n", m_result[3], m_result[4], m_result[5], m_result[0], m_result[1], m_result[2], k);
}


int main(int argc, char* argv[])
{
    Calc_Process(9.8);
    system("PAUSE");
    return 0;
}

#endif



#if METHOD == 3
//--------------------------method3------------------------------
#include "stdio.h"
#include "string.h"
#include "math.h"

#define MATRIX_SIZE 6
#define u8 unsigned char
double m_matrix[MATRIX_SIZE][MATRIX_SIZE + 1];//系数矩阵
double solve[MATRIX_SIZE] = { 0 };//方程组的解对应最小二乘椭球拟合中的，a，b，c，d，e，f，

double m_result[MATRIX_SIZE];
int N = 0;//计算累计的采样点次数的

          //取绝对值
double Abs(double a)
{
    return a < 0 ? -a : a;
}

//把矩阵系数全部清除为0
void ResetMatrix(void)
{
    for (u8 row = 0; row < MATRIX_SIZE; row++)
    {
        for (u8 column = 0; column < MATRIX_SIZE + 1; column++)
            m_matrix[row][column] = 0.0f;
    }
}

//把输入的数据先生成矩阵的元素的总和
void CalcData_Input(double x, double y, double z)
{
    double V[MATRIX_SIZE + 1];
    N++;
    V[0] = y*y;
    V[1] = z*z;
    V[2] = x;
    V[3] = y;
    V[4] = z;
    V[5] = 1.0;
    V[6] = -x*x;
    //构建系数矩阵，并进行累加
    for (u8 row = 0; row < MATRIX_SIZE; row++)
    {
        for (u8 column = 0; column < MATRIX_SIZE + 1; column++)
        {
            m_matrix[row][column] += V[row] * V[column];
        }
    }
    //b向量是m_matrix[row][6]
}

//化简系数矩阵，把除以N带上
void CalcData_Input_average()
{
    for (u8 row = 0; row < MATRIX_SIZE; row++)
        for (u8 column = 0; column < MATRIX_SIZE + 1; column++)
            m_matrix[row][column] /= N;
    //b向量是m_matrix[row][6]
}

//显示出来系数矩阵和增广矩阵[A|b]
void DispMatrix(void)
{
    for (u8 row = 0; row < MATRIX_SIZE; row++)
    {
        for (u8 column = 0; column < MATRIX_SIZE + 1; column++)
        {
            printf("%.3f ", m_matrix[row][column]);
            if (column == MATRIX_SIZE - 1)
                printf("|");
        }
        printf("\r\n");
    }
    printf("\r\n----------------------------\r\n");
}

//交换两行元素位置
void Row2_swop_Row1(int row1, int row2)
{
    double tmp = 0;
    for (u8 column = 0; column < MATRIX_SIZE + 1; column++)
    {
        tmp = m_matrix[row1][column];
        m_matrix[row1][column] = m_matrix[row2][column];
        m_matrix[row2][column] = tmp;
    }
}

//用把row行的元素乘以一个系数k
void k_muiltply_Row(double k, int row)
{
    for (u8 column = 0; column < MATRIX_SIZE + 1; column++)
        m_matrix[row][column] *= k;
}

//用一个数乘以row1行加到row2行上去
void Row2_add_kRow1(double k, int row1, int row2)
{
    for (u8 column = 0; column < MATRIX_SIZE + 1; column++)
        m_matrix[row2][column] += k*m_matrix[row1][column];
}


//列主元，第k次消元之前，把k行到MATRIX_SIZE的所有行进行冒泡排出最大，排序的依据是k列的元素的大小
void MoveBiggestElement_to_Top(int k)
{
    int row = 0, column = 0;

    for (row = k + 1; row < MATRIX_SIZE; row++)
    {
        if (Abs(m_matrix[k][k]) < Abs(m_matrix[row][k]))
        {
            Row2_swop_Row1(k, row);
        }
    }
}

//高斯消元法，求行阶梯型矩阵
u8 Matrix_GaussElimination(void)
{
    double k = 0;
    for (u8 cnt = 0; cnt < MATRIX_SIZE; cnt++)//进行第k次的运算，主要是针对k行以下的行数把k列的元素都变成0
    {
        //把k行依据k列的元素大小，进行排序
        MoveBiggestElement_to_Top(cnt);
        if (m_matrix[cnt][cnt] == 0)
            return(1);//返回值表示错误
                      //把k行下面的行元素全部消成0，整行变化
        for (u8 row = cnt + 1; row < MATRIX_SIZE; row++)
        {
            k = -m_matrix[row][cnt] / m_matrix[cnt][cnt];
            Row2_add_kRow1(k, cnt, row);
        }
        DispMatrix();
    }
    return 0;
}

//求行最简型矩阵，即把对角线的元素全部化成1
void Matrix_RowSimplify(void)
{
    double k = 0;
    for (u8 row = 0; row < MATRIX_SIZE; row++)
    {
        k = 1 / m_matrix[row][row];
        k_muiltply_Row(k, row);
    }
    DispMatrix();
}

//求非齐次线性方程组的解
void Matrix_Solve(double* solve)
{
    for (short row = MATRIX_SIZE - 1; row >= 0; row--)
    {
        solve[row] = m_matrix[row][MATRIX_SIZE];
        for (u8 column = MATRIX_SIZE - 1; column >= row + 1; column--)
            solve[row] -= m_matrix[row][column] * solve[column];
    }
    printf("  a = %f| b = %f| c = %f| d = %f| e = %f| f = %f ", solve[0], solve[1], solve[2], solve[3], solve[4], solve[5]);
    printf("\r\n");
    printf("\r\n");
}

//整个椭球校准的过程
void Ellipsoid_fitting_Process(void)
{
    ResetMatrix();

    //这里输入任意个点加速度参数，尽量在球面上均匀分布
    CalcData_Input(9.50, -1.09, 0.29);
    CalcData_Input(-9.92, -1.06, -0.15);
    CalcData_Input(-0.16, 8.71, -0.09);
    CalcData_Input(-0.32, -10.74, 0.96);
    CalcData_Input(-0.42, -1.15, 9.85);
    CalcData_Input(-1.08, -0.72, -9.85);

    CalcData_Input_average();//对输入的数据到矩阵元素进行归一化
    DispMatrix();//显示原始的增广矩阵
    if (Matrix_GaussElimination())	//求得行阶梯形矩阵
        printf("the marix could not be solved\r\n");
    else
    {
        Matrix_RowSimplify();//化行最简形态
        Matrix_Solve(solve);//求解a,b,c,d,e,f

        double a = 0, b = 0, c = 0, d = 0, e = 0, f = 0;
        a = solve[0];
        b = solve[1];
        c = solve[2];
        d = solve[3];
        e = solve[4];
        f = solve[5];

        double X0 = 0, Y0 = 0, Z0 = 0, A = 0, B = 0, C = 0;
        X0 = -c / 2;
        Y0 = -d / (2 * a);
        Z0 = -e / (2 * b);
        A = sqrt(X0*X0 + a*Y0*Y0 + b*Z0*Z0 - f);
        B = A / sqrt(a);
        C = A / sqrt(b);
        printf("  ((x - x0) / A) ^ 2 + ((y - y0) / B) ^ 2 + ((z - z0) / C) ^ 2 = 1 Ellipsoid result as below：\r\n");
        printf("\r\n");
        printf("  X0 = %f| Y0 = %f| Z0 = %f| A = %f| B = %f| C = %f \r\n", X0, Y0, Z0, A, B, C);
    }
    system("PAUSE");
}


int main(int argc, char* argv[])
{
    Ellipsoid_fitting_Process();
    return 0;
}
#endif
