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

    //����VxVt����(VtΪV��ת��)���������ۼ�
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

//��˹��Ԫ�������н����;���
u8 Matrix_GaussElimination(void)
{
    int row, column, i, j;
    double tmp;

    for (row = 0, column = 0; row<m - 1 && column<n - 1; row++, column++) {
        //����ǰ������һ��������
        MoveBiggestElement2Top(row, column);

        //���ж�Ϊ0
        if (Equal(m_matrix[row][column], 0.0f)) {
            printf("qiyi matrix:%d %d\r\n", row, column);
            //DispMatrix();
            //return 0;
            row--;
            continue;
        }

        //��˹��Ԫ
        for (i = row + 1; i<m; i++) {
            if (Equal(m_matrix[i][column], 0.0f))
                continue;	//Ϊ0�����账��

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

//��������;���
int Matrix_RowSimplify(void)
{
    int c = n;//����ֵ����ʾ(������ⳣ����+1)��
              //
    int row, column, k, s, t;
    double tmp;
    //
    for (row = 0, column = 0; row<m && column<n; row++, column++)
    {
        if (Equal(m_matrix[row][column], 0))//ƽ�ƣ��ҳ����е�һ�����㣻
        {
            row--;
            continue;
        }
        //
        c--;//��һ��������
            //
            //��a[i][j]Ϊ1��
        tmp = 1 / m_matrix[row][column];
        for (k = column; k<n; k++)//ǰ���"0"�Ͳ������ˣ�
            m_matrix[row][k] *= tmp;
        //
        //��a[s][j]Ϊ0
        for (s = 0; s<row; s++)//�����0Ҳ���ô���
        {
            if (Equal(m_matrix[s][column], 0))
                continue;//�Ѿ�Ϊ0��
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

    //�ҳ�������λ��
    memset(any_sol, 0, MATRIX_SIZE);
    for (row = 0, column = 0; row<m && column<n - 1; row++, column++) {
        if (Equal(m_matrix[row][column], 0.0f)) {
            any_sol[column] = 1;	//��¼������λ��
            row--;	//����1��
        }
    }

    //���
    row = 0;
    for (column = 0; column<n - 1; column++) {
        if (any_sol[column] == 1) {	//�����
            sol[column] = C[column];
        }
        else {
            sol[column] = m_matrix[row][n - 1];
            //���������
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

    //������������ų����������꣬�뾡����֤�������Ϸֲ�����
    CalcData_Input(9.50, -1.09, 0.29);
    CalcData_Input(-9.92, -1.06, -0.15);
    CalcData_Input(-0.16, 8.71, -0.09);
    CalcData_Input(-0.32, -10.74, 0.96);
    //CalcData_Input(-0.42, -1.15, 9.85);
    //CalcData_Input(-1.08, -0.72, -9.85);
    DispMatrix();
    Matrix_GaussElimination();
    Matrix_RowSimplify();

    //��ֵ��������ֵ
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
double m_matrix[MATRIX_SIZE][MATRIX_SIZE + 1];//ϵ������
double solve[MATRIX_SIZE] = { 0 };//������Ľ��Ӧ��С������������еģ�a��b��c��d��e��f��

double m_result[MATRIX_SIZE];
int N = 0;//�����ۼƵĲ����������

          //ȡ����ֵ
double Abs(double a)
{
    return a < 0 ? -a : a;
}

//�Ѿ���ϵ��ȫ�����Ϊ0
void ResetMatrix(void)
{
    for (u8 row = 0; row < MATRIX_SIZE; row++)
    {
        for (u8 column = 0; column < MATRIX_SIZE + 1; column++)
            m_matrix[row][column] = 0.0f;
    }
}

//����������������ɾ����Ԫ�ص��ܺ�
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
    //����ϵ�����󣬲������ۼ�
    for (u8 row = 0; row < MATRIX_SIZE; row++)
    {
        for (u8 column = 0; column < MATRIX_SIZE + 1; column++)
        {
            m_matrix[row][column] += V[row] * V[column];
        }
    }
    //b������m_matrix[row][6]
}

//����ϵ�����󣬰ѳ���N����
void CalcData_Input_average()
{
    for (u8 row = 0; row < MATRIX_SIZE; row++)
        for (u8 column = 0; column < MATRIX_SIZE + 1; column++)
            m_matrix[row][column] /= N;
    //b������m_matrix[row][6]
}

//��ʾ����ϵ��������������[A|b]
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

//��������Ԫ��λ��
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

//�ð�row�е�Ԫ�س���һ��ϵ��k
void k_muiltply_Row(double k, int row)
{
    for (u8 column = 0; column < MATRIX_SIZE + 1; column++)
        m_matrix[row][column] *= k;
}

//��һ��������row1�мӵ�row2����ȥ
void Row2_add_kRow1(double k, int row1, int row2)
{
    for (u8 column = 0; column < MATRIX_SIZE + 1; column++)
        m_matrix[row2][column] += k*m_matrix[row1][column];
}


//����Ԫ����k����Ԫ֮ǰ����k�е�MATRIX_SIZE�������н���ð���ų���������������k�е�Ԫ�صĴ�С
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

//��˹��Ԫ�������н����;���
u8 Matrix_GaussElimination(void)
{
    double k = 0;
    for (u8 cnt = 0; cnt < MATRIX_SIZE; cnt++)//���е�k�ε����㣬��Ҫ�����k�����µ�������k�е�Ԫ�ض����0
    {
        //��k������k�е�Ԫ�ش�С����������
        MoveBiggestElement_to_Top(cnt);
        if (m_matrix[cnt][cnt] == 0)
            return(1);//����ֵ��ʾ����
                      //��k���������Ԫ��ȫ������0�����б仯
        for (u8 row = cnt + 1; row < MATRIX_SIZE; row++)
        {
            k = -m_matrix[row][cnt] / m_matrix[cnt][cnt];
            Row2_add_kRow1(k, cnt, row);
        }
        DispMatrix();
    }
    return 0;
}

//��������;��󣬼��ѶԽ��ߵ�Ԫ��ȫ������1
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

//���������Է�����Ľ�
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

//��������У׼�Ĺ���
void Ellipsoid_fitting_Process(void)
{
    ResetMatrix();

    //�����������������ٶȲ����������������Ͼ��ȷֲ�
    CalcData_Input(9.50, -1.09, 0.29);
    CalcData_Input(-9.92, -1.06, -0.15);
    CalcData_Input(-0.16, 8.71, -0.09);
    CalcData_Input(-0.32, -10.74, 0.96);
    CalcData_Input(-0.42, -1.15, 9.85);
    CalcData_Input(-1.08, -0.72, -9.85);

    CalcData_Input_average();//����������ݵ�����Ԫ�ؽ��й�һ��
    DispMatrix();//��ʾԭʼ���������
    if (Matrix_GaussElimination())	//����н����ξ���
        printf("the marix could not be solved\r\n");
    else
    {
        Matrix_RowSimplify();//���������̬
        Matrix_Solve(solve);//���a,b,c,d,e,f

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
        printf("  ((x - x0) / A) ^ 2 + ((y - y0) / B) ^ 2 + ((z - z0) / C) ^ 2 = 1 Ellipsoid result as below��\r\n");
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
