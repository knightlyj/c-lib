#include "quaternion.h"
#include "stdio.h"

static void UpdateMatrix(Quaternion* q)
{
    q->mat[0][0] = 1 - 2 * (q->y * q->y + q->z * q->z);
    q->mat[0][1] = 2 * (q->x * q->y - q->w * q->z);
    q->mat[0][2] = 2 * (q->x * q->z + q->w * q->y);

    q->mat[1][0] = 2 * (q->x * q->y + q->w * q->z);
    q->mat[1][1] = 1 - 2 * (q->x * q->x + q->z * q->z);
    q->mat[1][2] = 2 * (q->y * q->z - q->w * q->x);

    q->mat[2][0] = 2 * (q->x * q->z - q->w * q->y);
    q->mat[2][1] = 2 * (q->y * q->z + q->w * q->x);
    q->mat[2][2] = 1 - 2 * (q->x * q->x + q->y * q->y);
}


float Quaternion_Magnitude(const Quaternion* q)
{
    return sqrtf(q->x * q->x
        + q->y * q->y
        + q->z * q->z
        + q->w * q->w);
}


void Quaternion_Normalize(Quaternion* q)
{
    float s = 1.0f / Quaternion_Magnitude(q);

    q->x *= s;
    q->y *= s;
    q->z *= s;
    q->w *= s;

    UpdateMatrix(q);
}

void Quaternion_ToRadianAxis(Quaternion* q, Vector3* axis, float* theta)
{
    *theta = acosf(q->w) * 2;

    axis->x = q->x;
    axis->y = q->y;
    axis->z = q->z;

    Vector3_Normalize(axis);
}

void Quaternion_FromRadianAxis(Quaternion* q, Vector3* axis, float theta, uint8_t normalized)
{
    float cosHalfTheta, sinHalfTheta, halfTheta;
    if (!normalized)
        Vector3_Normalize(axis);

    halfTheta = theta / 2.0f;
    cosHalfTheta = cosf(halfTheta);
    sinHalfTheta = sinf(halfTheta);

    q->w = cosHalfTheta;
    q->x = sinHalfTheta * axis->x;
    q->y = sinHalfTheta * axis->y;
    q->z = sinHalfTheta * axis->z;

    UpdateMatrix(q);
}


void Quaternion_Rotate(const Quaternion* q, const Vector3* v, Vector3* out)
{
    float x, y, z;
    x = q->mat[0][0] * v->x + q->mat[0][1] * v->y + q->mat[0][2] * v->z;
    y = q->mat[1][0] * v->x + q->mat[1][1] * v->y + q->mat[1][2] * v->z;
    z = q->mat[2][0] * v->x + q->mat[2][1] * v->y + q->mat[2][2] * v->z;

    out->x = x;
    out->y = y;
    out->z = z;
}

void Quaternion_Multiply(const Quaternion* q1, const Quaternion* q2, Quaternion* out)
{
    Vector3 v;
    float w;
    w = q1->w * q2->w - Vector3_Dot((Vector3*)q1, (Vector3*)q2);

    Vector3_Cross((Vector3*)q1, (Vector3*)q2, &v);

    v.x += q1->w * q2->x + q2->w * q1->x;
    v.y += q1->w * q2->y + q2->w * q1->y;
    v.z += q1->w * q2->z + q2->w * q1->z;

    out->x = v.x;
    out->y = v.y;
    out->z = v.z;
    out->w = w;

    UpdateMatrix(out);
}

uint8_t Quaternion_Equal(const Quaternion* q1, const Quaternion* q2)
{
    float x, y, z, w;
    float sqrMagnitude;
    x = q1->x - q2->x;
    y = q1->y - q2->y;
    z = q1->z - q2->z;
    w = q1->w - q2->w;

    sqrMagnitude = x * x + y * y + z * z + w * w;
    return FLOAT_EQUAL(sqrMagnitude, 0);
}

void Quaternion_ToString(const Quaternion* q, char* buff)
{
    sprintf(buff, "{%.2f, (%.2f, %.2f, %.2f)}", q->w, q->x, q->y, q->z);
}

