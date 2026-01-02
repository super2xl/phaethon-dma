#pragma once
#include "vector.h"
#include <cstring>

namespace phaethon {
namespace math {

// 4x4 Matrix (column-major for graphics)
struct Matrix4x4 {
    float m[4][4];

    Matrix4x4() {
        std::memset(m, 0, sizeof(m));
    }

    Matrix4x4(float m00, float m01, float m02, float m03,
              float m10, float m11, float m12, float m13,
              float m20, float m21, float m22, float m23,
              float m30, float m31, float m32, float m33) {
        m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
        m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
        m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
        m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
    }

    static Matrix4x4 Identity() {
        Matrix4x4 result;
        result.m[0][0] = 1.f;
        result.m[1][1] = 1.f;
        result.m[2][2] = 1.f;
        result.m[3][3] = 1.f;
        return result;
    }

    float* operator[](int i) { return m[i]; }
    const float* operator[](int i) const { return m[i]; }

    Vector4 operator*(const Vector4& v) const {
        return Vector4(
            m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w,
            m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
            m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
            m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w
        );
    }

    Matrix4x4 operator*(const Matrix4x4& other) const {
        Matrix4x4 result;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result.m[i][j] = 0.f;
                for (int k = 0; k < 4; k++) {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }
        return result;
    }

    // Transform a 3D point
    Vector3 TransformPoint(const Vector3& point) const {
        Vector4 v4(point, 1.f);
        Vector4 result = (*this) * v4;
        return Vector3(result.x, result.y, result.z);
    }

    // Transform a 3D vector (direction)
    Vector3 TransformVector(const Vector3& vec) const {
        Vector4 v4(vec, 0.f);
        Vector4 result = (*this) * v4;
        return Vector3(result.x, result.y, result.z);
    }
};

// View matrix - specifically for CS2 (stored as float[16])
struct ViewMatrix {
    float matrix[16];

    ViewMatrix() {
        std::memset(matrix, 0, sizeof(matrix));
    }

    ViewMatrix(const float* mat) {
        std::memcpy(matrix, mat, sizeof(matrix));
    }

    // Access as 4x4
    float& At(int row, int col) {
        return matrix[row * 4 + col];
    }

    const float& At(int row, int col) const {
        return matrix[row * 4 + col];
    }

    // Convert to Matrix4x4 for operations
    Matrix4x4 ToMatrix4x4() const {
        return Matrix4x4(
            matrix[0],  matrix[1],  matrix[2],  matrix[3],
            matrix[4],  matrix[5],  matrix[6],  matrix[7],
            matrix[8],  matrix[9],  matrix[10], matrix[11],
            matrix[12], matrix[13], matrix[14], matrix[15]
        );
    }

    // Screen transform (world to NDC)
    bool ScreenTransform(const Vector3& world, Vector3& ndc) const {
        // Multiply world position by view-projection matrix
        ndc.x = matrix[0] * world.x + matrix[1] * world.y + matrix[2] * world.z + matrix[3];
        ndc.y = matrix[4] * world.x + matrix[5] * world.y + matrix[6] * world.z + matrix[7];

        float w = matrix[12] * world.x + matrix[13] * world.y + matrix[14] * world.z + matrix[15];

        // Check if behind camera
        if (w < 0.001f) {
            return false;
        }

        // Perspective divide
        float invW = 1.f / w;
        ndc.x *= invW;
        ndc.y *= invW;

        return true;
    }

    // World to screen (final 2D screen coordinates)
    bool WorldToScreen(const Vector3& world, Vector2& screen, int screenWidth, int screenHeight) const {
        Vector3 ndc;
        if (!ScreenTransform(world, ndc)) {
            return false;
        }

        // NDC to screen coordinates
        screen.x = (screenWidth / 2.0f) * (1.0f + ndc.x);
        screen.y = (screenHeight / 2.0f) * (1.0f - ndc.y); // Flip Y for screen space

        return true;
    }
};

} // namespace math
} // namespace phaethon
