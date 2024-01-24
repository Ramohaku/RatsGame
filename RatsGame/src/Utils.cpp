#include "stdafx.h"
#include "Utils.h"

float linearInterpolation(float a, float b, float t)
{
	return a + (b - a) * t;
}

std::optional<Intersection> getIntersection(const Vec2f& p1, const Vec2f& p2, const Vec2f& p3, const Vec2f& p4)
{
	const float tTop = (p4.x - p3.x) * (p1.y - p3.y) - (p4.y - p3.y) * (p1.x - p3.x);
	const float uTop = (p3.y - p1.y) * (p1.x - p2.x) - (p3.x - p1.x) * (p1.y - p2.y);
	const float bottom = (p4.y - p3.y) * (p2.x - p1.x) - (p4.x - p3.x) * (p2.y - p1.y);

	if (bottom != 0.0f)
	{
		const float offset01 = tTop / bottom;
		const float offset2 = uTop / bottom;
		if (offset01 >= 0.0f && offset01 <= 1.0f && offset2 >= 0.0f && offset2 <= 1.0f)
		{
			return std::optional<Intersection>({
				Vec2f{
					linearInterpolation(p1.x, p2.x, offset01),
					linearInterpolation(p1.y, p2.y, offset01)
				},
				offset01
				});
		}
	}

	return std::nullopt;
}

size_t getClosest(const std::array<float, COL_RAYS>& arr, size_t index1, size_t index2, float target)
{
	if (target - arr[index1] >= arr[index2] - target)
		return index1;
	else
		return index2;
}

size_t findClosest(const std::array<float, COL_RAYS>& arr, float target)
{
	if (target <= arr.front())
		return 0;
	if (target >= arr.back())
		return arr.size() - 1;

	int i = 0, j = arr.size(), mid = 0;
	while (i < j)
	{
		mid = (i + j) / 2;

		if (arr[mid] == target)
			return arr[mid];

		if (target < arr[mid])
		{
			if (mid > 0 && target > arr[mid - 1])
				return getClosest(arr, mid - 1, mid, target);

			j = mid;
		}

		else
		{
			if (mid < arr.size() - 1 && target < arr[mid + 1])
				return getClosest(arr, mid, mid + 1, target);

			i = mid + 1;
		}
	}

	return mid;
}