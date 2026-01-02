#pragma once
#include "vector.h"
#include "matrix.h"
#include <numbers>

namespace phaethon {
namespace math {

// Constants
constexpr float PI = std::numbers::pi_v<float>;
constexpr float DEG2RAD = PI / 180.f;
constexpr float RAD2DEG = 180.f / PI;

// Calculate angle from source to destination
inline QAngle CalcAngle(const Vector3& src, const Vector3& dst) {
    Vector3 delta = dst - src;
    double hyp = std::sqrt(delta.x * delta.x + delta.y * delta.y);

    QAngle angle;
    angle.pitch = std::atan(delta.z / hyp) * RAD2DEG;
    angle.yaw = std::atan2(delta.y, delta.x) * RAD2DEG;
    angle.roll = 0.f;

    return angle.Normalized();
}

// Calculate FOV between two angles
inline float GetFOV(const QAngle& viewAngles, const QAngle& aimAngles) {
    QAngle delta = aimAngles - viewAngles;
    delta.Normalize();

    return std::sqrt(delta.pitch * delta.pitch + delta.yaw * delta.yaw);
}

// Convert angle to forward vector
inline Vector3 AngleToVector(const QAngle& angle) {
    float pitchRad = angle.pitch * DEG2RAD;
    float yawRad = angle.yaw * DEG2RAD;

    float cosPitch = std::cos(pitchRad);
    float sinPitch = std::sin(pitchRad);
    float cosYaw = std::cos(yawRad);
    float sinYaw = std::sin(yawRad);

    return Vector3(
        cosPitch * cosYaw,
        cosPitch * sinYaw,
        -sinPitch
    );
}

// Convert forward vector to angle
inline QAngle VectorToAngle(const Vector3& forward) {
    QAngle angle;

    if (forward.x == 0.f && forward.y == 0.f) {
        angle.pitch = (forward.z > 0.f) ? -90.f : 90.f;
        angle.yaw = 0.f;
    } else {
        angle.pitch = std::atan2(-forward.z, forward.Length2D()) * RAD2DEG;
        angle.yaw = std::atan2(forward.y, forward.x) * RAD2DEG;
    }

    angle.roll = 0.f;
    return angle.Normalized();
}

// 2D Bounding Box (screen space)
struct Rect {
    float x, y;       // Top-left corner
    float width, height;

    Rect() : x(0.f), y(0.f), width(0.f), height(0.f) {}
    Rect(float x, float y, float w, float h) : x(x), y(y), width(w), height(h) {}

    float Left() const { return x; }
    float Right() const { return x + width; }
    float Top() const { return y; }
    float Bottom() const { return y + height; }

    Vector2 TopLeft() const { return Vector2(x, y); }
    Vector2 TopRight() const { return Vector2(x + width, y); }
    Vector2 BottomLeft() const { return Vector2(x, y + height); }
    Vector2 BottomRight() const { return Vector2(x + width, y + height); }
    Vector2 Center() const { return Vector2(x + width * 0.5f, y + height * 0.5f); }

    bool Contains(const Vector2& point) const {
        return point.x >= x && point.x <= (x + width) &&
               point.y >= y && point.y <= (y + height);
    }

    bool Intersects(const Rect& other) const {
        return !(Right() < other.Left() || Left() > other.Right() ||
                 Bottom() < other.Top() || Top() > other.Bottom());
    }

    void Expand(float amount) {
        x -= amount;
        y -= amount;
        width += amount * 2.f;
        height += amount * 2.f;
    }

    bool IsValid() const { return width > 0.f && height > 0.f; }
};

// 3D Axis-Aligned Bounding Box
struct AABB {
    Vector3 min, max;

    AABB() : min(0.f, 0.f, 0.f), max(0.f, 0.f, 0.f) {}
    AABB(const Vector3& min, const Vector3& max) : min(min), max(max) {}

    Vector3 Center() const { return (min + max) * 0.5f; }
    Vector3 Size() const { return max - min; }

    // Get all 8 corners
    void GetCorners(Vector3 corners[8]) const {
        corners[0] = Vector3(min.x, min.y, min.z);
        corners[1] = Vector3(min.x, max.y, min.z);
        corners[2] = Vector3(max.x, max.y, min.z);
        corners[3] = Vector3(max.x, min.y, min.z);
        corners[4] = Vector3(max.x, max.y, max.z);
        corners[5] = Vector3(min.x, max.y, max.z);
        corners[6] = Vector3(min.x, min.y, max.z);
        corners[7] = Vector3(max.x, min.y, max.z);
    }

    // Project to 2D screen space
    bool ToScreenRect(const ViewMatrix& viewMatrix, Rect& outRect, int screenWidth, int screenHeight) const {
        Vector3 corners[8];
        GetCorners(corners);

        // Project all 8 corners
        Vector2 screenPoints[8];
        bool anyVisible = false;

        for (int i = 0; i < 8; i++) {
            if (viewMatrix.WorldToScreen(corners[i], screenPoints[i], screenWidth, screenHeight)) {
                anyVisible = true;
            } else {
                // If corner is behind camera, still might be partially visible
                // For now, mark as invalid
                return false;
            }
        }

        if (!anyVisible) {
            return false;
        }

        // Find min/max screen coordinates
        float minX = screenPoints[0].x;
        float maxX = screenPoints[0].x;
        float minY = screenPoints[0].y;
        float maxY = screenPoints[0].y;

        for (int i = 1; i < 8; i++) {
            minX = std::min(minX, screenPoints[i].x);
            maxX = std::max(maxX, screenPoints[i].x);
            minY = std::min(minY, screenPoints[i].y);
            maxY = std::max(maxY, screenPoints[i].y);
        }

        outRect.x = minX;
        outRect.y = minY;
        outRect.width = maxX - minX;
        outRect.height = maxY - minY;

        return outRect.IsValid();
    }

    bool IsValid() const { return max.x > min.x && max.y > min.y && max.z > min.z; }
};

// Distance helpers
inline float Distance3D(const Vector3& a, const Vector3& b) {
    return a.Distance(b);
}

inline float Distance2D(const Vector3& a, const Vector3& b) {
    return Vector2(a.x - b.x, a.y - b.y).Length();
}

// Interpolation
inline float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

inline Vector3 Lerp(const Vector3& a, const Vector3& b, float t) {
    return Vector3(
        Lerp(a.x, b.x, t),
        Lerp(a.y, b.y, t),
        Lerp(a.z, b.z, t)
    );
}

inline QAngle Lerp(const QAngle& a, const QAngle& b, float t) {
    QAngle result(
        Lerp(a.pitch, b.pitch, t),
        Lerp(a.yaw, b.yaw, t),
        Lerp(a.roll, b.roll, t)
    );
    result.Normalize();
    return result;
}

} // namespace math
} // namespace phaethon
