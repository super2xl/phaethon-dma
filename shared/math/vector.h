#pragma once
#include <cmath>
#include <algorithm>

namespace phaethon {
namespace math {

struct Vector2 {
    float x, y;

    Vector2() : x(0.f), y(0.f) {}
    Vector2(float x, float y) : x(x), y(y) {}

    Vector2 operator+(const Vector2& v) const { return Vector2(x + v.x, y + v.y); }
    Vector2 operator-(const Vector2& v) const { return Vector2(x - v.x, y - v.y); }
    Vector2 operator*(float scalar) const { return Vector2(x * scalar, y * scalar); }
    Vector2 operator/(float scalar) const { return Vector2(x / scalar, y / scalar); }

    Vector2& operator+=(const Vector2& v) { x += v.x; y += v.y; return *this; }
    Vector2& operator-=(const Vector2& v) { x -= v.x; y -= v.y; return *this; }
    Vector2& operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }
    Vector2& operator/=(float scalar) { x /= scalar; y /= scalar; return *this; }

    float Length() const { return std::sqrt(x * x + y * y); }
    float LengthSqr() const { return x * x + y * y; }
    float Distance(const Vector2& v) const { return (*this - v).Length(); }
    float DistanceSqr(const Vector2& v) const { return (*this - v).LengthSqr(); }

    Vector2 Normalized() const {
        float len = Length();
        return (len > 0.f) ? (*this / len) : Vector2(0.f, 0.f);
    }

    float Dot(const Vector2& v) const { return x * v.x + y * v.y; }

    bool IsZero() const { return x == 0.f && y == 0.f; }
};

struct Vector3 {
    float x, y, z;

    Vector3() : x(0.f), y(0.f), z(0.f) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

    Vector3 operator+(const Vector3& v) const { return Vector3(x + v.x, y + v.y, z + v.z); }
    Vector3 operator-(const Vector3& v) const { return Vector3(x - v.x, y - v.y, z - v.z); }
    Vector3 operator*(float scalar) const { return Vector3(x * scalar, y * scalar, z * scalar); }
    Vector3 operator/(float scalar) const { return Vector3(x / scalar, y / scalar, z / scalar); }
    Vector3 operator-() const { return Vector3(-x, -y, -z); }

    Vector3& operator+=(const Vector3& v) { x += v.x; y += v.y; z += v.z; return *this; }
    Vector3& operator-=(const Vector3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    Vector3& operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }
    Vector3& operator/=(float scalar) { x /= scalar; y /= scalar; z /= scalar; return *this; }

    bool operator==(const Vector3& v) const { return x == v.x && y == v.y && z == v.z; }
    bool operator!=(const Vector3& v) const { return !(*this == v); }

    float Length() const { return std::sqrt(x * x + y * y + z * z); }
    float LengthSqr() const { return x * x + y * y + z * z; }
    float Length2D() const { return std::sqrt(x * x + y * y); }
    float Length2DSqr() const { return x * x + y * y; }

    float Distance(const Vector3& v) const { return (*this - v).Length(); }
    float DistanceSqr(const Vector3& v) const { return (*this - v).LengthSqr(); }

    Vector3 Normalized() const {
        float len = Length();
        return (len > 0.f) ? (*this / len) : Vector3(0.f, 0.f, 0.f);
    }

    void Normalize() {
        float len = Length();
        if (len > 0.f) {
            x /= len;
            y /= len;
            z /= len;
        }
    }

    float Dot(const Vector3& v) const { return x * v.x + y * v.y + z * v.z; }

    Vector3 Cross(const Vector3& v) const {
        return Vector3(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }

    bool IsZero() const { return x == 0.f && y == 0.f && z == 0.f; }

    // Useful for world positions
    Vector2 To2D() const { return Vector2(x, y); }
};

struct Vector4 {
    float x, y, z, w;

    Vector4() : x(0.f), y(0.f), z(0.f), w(0.f) {}
    Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    Vector4(const Vector3& v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}

    Vector4 operator+(const Vector4& v) const { return Vector4(x + v.x, y + v.y, z + v.z, w + v.w); }
    Vector4 operator-(const Vector4& v) const { return Vector4(x - v.x, y - v.y, z - v.z, w - v.w); }
    Vector4 operator*(float scalar) const { return Vector4(x * scalar, y * scalar, z * scalar, w * scalar); }
    Vector4 operator/(float scalar) const { return Vector4(x / scalar, y / scalar, z / scalar, w / scalar); }

    Vector3 ToVector3() const { return Vector3(x, y, z); }
};

// Angles (pitch, yaw, roll in degrees)
struct QAngle {
    float pitch, yaw, roll;

    QAngle() : pitch(0.f), yaw(0.f), roll(0.f) {}
    QAngle(float pitch, float yaw, float roll = 0.f) : pitch(pitch), yaw(yaw), roll(roll) {}

    QAngle operator+(const QAngle& a) const { return QAngle(pitch + a.pitch, yaw + a.yaw, roll + a.roll); }
    QAngle operator-(const QAngle& a) const { return QAngle(pitch - a.pitch, yaw - a.yaw, roll - a.roll); }
    QAngle operator*(float scalar) const { return QAngle(pitch * scalar, yaw * scalar, roll * scalar); }
    QAngle operator/(float scalar) const { return QAngle(pitch / scalar, yaw / scalar, roll / scalar); }

    QAngle& operator+=(const QAngle& a) { pitch += a.pitch; yaw += a.yaw; roll += a.roll; return *this; }
    QAngle& operator-=(const QAngle& a) { pitch -= a.pitch; yaw -= a.yaw; roll -= a.roll; return *this; }

    void Normalize() {
        // Clamp pitch to [-89, 89]
        pitch = std::clamp(pitch, -89.f, 89.f);

        // Normalize yaw to [-180, 180]
        while (yaw > 180.f) yaw -= 360.f;
        while (yaw < -180.f) yaw += 360.f;

        // Roll usually stays 0 in FPS games
        roll = 0.f;
    }

    QAngle Normalized() const {
        QAngle result = *this;
        result.Normalize();
        return result;
    }

    bool IsZero() const { return pitch == 0.f && yaw == 0.f && roll == 0.f; }
};

} // namespace math
} // namespace phaethon
