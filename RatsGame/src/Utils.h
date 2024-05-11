#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#define FPS 120.0f
#define FRAME_TIME_MS 1000.0f / FPS

#define PI_F 3.14159265359f

#define COL_RAYS 16

template <typename T>
struct Vec2
{
	T x, y;

	Vec2 operator+(const Vec2& other) const
	{
		return Vec2{ x + other.x, y + other.y };
	}

	Vec2 operator-(const Vec2& other) const
	{
		return Vec2{ x - other.x, y - other.y };
	}

	Vec2 operator-() const
	{
		return Vec2{ -x, -y };
	}

	Vec2 operator*(const Vec2& other) const
	{
		return Vec2{ x * other.x, y * other.y };
	}

	Vec2 operator/(T value) const
	{
		return Vec2{ x / value, y / value };
	}
};
using Vec2f = Vec2<float>;
using Vec2d = Vec2<double>;

template <typename T>
struct Vec3
{
	union { T x, r; };
	union { T y, g; };
	union { T z, b; };
};
using Vec3f = Vec3<float>;

template <typename T>
struct Vec4
{
	union { T x, r; };
	union { T y, g; };
	union { T z, b; };
	union { T w, a; };
};
using Vec4f = Vec4<float>;



inline Vec2f operator*(const glm::mat2& matrix, const Vec2f& vec)
{
	glm::vec2 glmVec{ vec.x, vec.y };
	glm::vec2 glmRes = matrix * glmVec;
	return Vec2f{ glmRes.x, glmRes.y };
}


struct Intersection
{
	Vec2f point;
	float offset01;
};

float linearInterpolation(float a, float b, float t);
std::optional<Intersection> getIntersection(const Vec2f& p1, const Vec2f& p2, const Vec2f& p3, const Vec2f& p4);

size_t getClosest(const std::array<float, COL_RAYS>& arr, size_t index1, size_t index2, float target);
size_t findClosest(const std::array<float, COL_RAYS>& arr, float target);

int randInt(int min, int max);

inline float dotProduct(const Vec2f& vec1, const Vec2f& vec2)
{
	return vec1.x * vec2.x + vec1.y * vec2.y;
}

inline float magnitude(const Vec2f& vec)
{
	return sqrt(vec.x * vec.x + vec.y * vec.y);
}

float getAngleBetweenLineAndSegment(float lineAngle, const Vec2f& pos1, const Vec2f& pos2);

void correctAngleTooSmall(float& angle);
void correctAngleTooBig(float& angle);
void correctAngleOneIter(float& angle);
void correctAngleManyIter(float& angle);

inline float crossProduct(const Vec2f& vec1, const Vec2f& vec2)
{
	return vec1.x * vec2.y - vec1.y * vec2.x;
}