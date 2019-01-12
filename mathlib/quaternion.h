#pragma once

#ifdef __cplusplus
extern "C" {
#endif

//---------header start---------------

#include "mathex.h"
#include "vector3.h"
#include "stdint.h"


typedef struct
{
    float x, y, z, w;

    float mat[3][3]; //旋转矩阵
} Quaternion;

//取摸
float Quaternion_Magnitude(const Quaternion* q);

//标准化
void Quaternion_Normalize(Quaternion* q);

//转换为轴和弧度
void Quaternion_ToRadianAxis(Quaternion* q, Vector3* axis, float* theta);

//从轴-弧度获取四元数
void Quaternion_FromRadianAxis(Quaternion* q, Vector3* axis, float theta, uint8_t normalized);

//旋转v,out可以是v
void Quaternion_Rotate(const Quaternion* q, const Vector3* v, Vector3* out);

//相乘,out不能为NULL,可以为q1或q2或其他值
void Quaternion_Multiply(const Quaternion* q1, const Quaternion* q2, Quaternion* out);

//判断相等
uint8_t Quaternion_Equal(const Quaternion* q1, const Quaternion* q2);

void Quaternion_ToString(const Quaternion* q, char* buff);

//----------header end--------------

#ifdef __cplusplus
}
#endif


