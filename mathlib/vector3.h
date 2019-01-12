#pragma once



#ifdef __cplusplus
extern "C" {
#endif

//---------header start---------------

#include "stdint.h"


typedef struct
{
    float x, y, z;
} Vector3;

//点乘
float Vector3_Dot(const Vector3* v1, const Vector3* v2);

//叉乘,out可以为v1或v2
Vector3* Vector3_Cross(const Vector3* v1, const Vector3* v2, Vector3* out);

//取模
float Vector3_Magnitude(const Vector3* v);

//模的平方
float Vector3_SqrMagnitude(const Vector3* v);

//标准化
void Vector3_Normalize(Vector3* v);

//标量乘,out可以为v
Vector3* Vector3_Scale(const Vector3* v, float s, Vector3* out);

//分量相乘,out可以为v1或v2
Vector3* Vector3_Multiply(const Vector3* v1, const Vector3* v2, Vector3* out);

//分量相加,out可以为v1或v2
Vector3* Vector3_Add(const Vector3* v1, const Vector3* v2, Vector3* out);

//分量相减,out可以为v1或v2
Vector3* Vector3_Subtract(const Vector3* v1, const Vector3* v2, Vector3* out);

//计算向量夹角
float Vector3_Radian(const Vector3* v1, const Vector3* v2);

//同上,但返回cos值
float Vector3_Cos(const Vector3* v1, const Vector3* v2);

//从v1到v2投影,out可以为v1或v2
Vector3* Vector3_Project(const Vector3* v1, const Vector3* v2, Vector3* out);

//从v1到normal所在平面投影,out可以为v1或normal
Vector3* Vector3_ProjectOnPlane(const Vector3* v1, const Vector3* normal, Vector3* out);

//判断相等
uint8_t Vector3_Equal(const Vector3* v1, const Vector3* v2);

//打印
char* Vector3_ToString(const Vector3* v, char* buff);

//----------header end--------------   

#ifdef __cplusplus
}
#endif
