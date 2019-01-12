#pragma once



#ifdef __cplusplus
extern "C" {
#endif

//---------header start---------------

#include "math.h"

#define INFINIT_SMALL   (0.000001f)

//判断浮点数相等
#define FLOAT_EQUAL(f1, f2)     (fabs(f1 - f2) < INFINIT_SMALL)

//判断浮点数接近
#define FLOAT_NEAR(f1, f2, tolerance) (fabs(f1 - f2) < tolerance)

//角度转为弧度
#define ANGLE_TO_RADIAN(angle)          ((float)angle * M_PI / 180.0f)

//弧度转为角度
#define RADIAN_TO_ANGLE(radian)         (radian * 180 / M_PI)   

//重力
#define GRAVITY       (9.80665f)

//----------header end--------------


#ifdef __cplusplus
}
#endif
