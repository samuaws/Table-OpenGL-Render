#ifndef MAT4_H
#define MAT4_H

#include <cmath>

struct Mat4
{
    float m[16];

    static Mat4 identity()
    {
        Mat4 result = {};
        result.m[0] = 1.0f; result.m[5] = 1.0f; result.m[10] = 1.0f; result.m[15] = 1.0f;
        return result;
    }

    static Mat4 perspective(float fov, float aspect, float near, float far)
    {
        Mat4 result = {};
        float tanHalfFov = tanf(fov * 0.5f);
        result.m[0] = 1.0f / (aspect * tanHalfFov);
        result.m[5] = 1.0f / tanHalfFov;
        result.m[10] = -(far + near) / (far - near);
        result.m[11] = -1.0f;
        result.m[14] = -(2.0f * far * near) / (far - near);
        return result;
    }

    static Mat4 lookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ)
    {
        float fx = centerX - eyeX, fy = centerY - eyeY, fz = centerZ - eyeZ;
        float rlf = 1.0f / sqrtf(fx * fx + fy * fy + fz * fz);
        fx *= rlf; fy *= rlf; fz *= rlf;

        float sx = upY * fz - upZ * fy, sy = upZ * fx - upX * fz, sz = upX * fy - upY * fx;
        float rls = 1.0f / sqrtf(sx * sx + sy * sy + sz * sz);
        sx *= rls; sy *= rls; sz *= rls;

        float ux = fy * sz - fz * sy, uy = fz * sx - fx * sz, uz = fx * sy - fy * sx;

        Mat4 result = identity();
        result.m[0] = sx; result.m[4] = sy; result.m[8] = sz;
        result.m[1] = ux; result.m[5] = uy; result.m[9] = uz;
        result.m[2] = -fx; result.m[6] = -fy; result.m[10] = -fz;
        result.m[12] = -(sx * eyeX + sy * eyeY + sz * eyeZ);
        result.m[13] = -(ux * eyeX + uy * eyeY + uz * eyeZ);
        result.m[14] = fx * eyeX + fy * eyeY + fz * eyeZ;
        return result;
    }

    static Mat4 translate(const Mat4& mat, float x, float y, float z)
    {
        Mat4 result = mat;
        result.m[12] += x;
        result.m[13] += y;
        result.m[14] += z;
        return result;
    }

    static Mat4 rotateX(const Mat4& mat, float angle)
    {
        Mat4 result = mat;
        float c = cosf(angle);
        float s = sinf(angle);
        result.m[5] = c; result.m[6] = -s;
        result.m[9] = s; result.m[10] = c;
        return result;
    }

    static Mat4 rotateY(const Mat4& mat, float angle)
    {
        Mat4 result = mat;
        float c = cosf(angle);
        float s = sinf(angle);
        result.m[0] = c; result.m[2] = s;
        result.m[8] = -s; result.m[10] = c;
        return result;
    }

    static Mat4 rotateZ(const Mat4& mat, float angle)
    {
        Mat4 result = mat;
        float c = cosf(angle);
        float s = sinf(angle);
        result.m[0] = c; result.m[1] = -s;
        result.m[4] = s; result.m[5] = c;
        return result;
    }
};

#endif // MAT4_H
