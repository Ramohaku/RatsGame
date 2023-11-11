#pragma once
#include "Rat.h"

struct Edge
{
	Vec2f pos;
	float dist;
};

class EdgeArray
{
public:
	EdgeArray(const Vec2f& ratCenter, Sprite* sprite);

	ptrdiff_t getMinIndex() const;

	const Edge& operator[](size_t index) const
	{
		return m_edges[index];
	}

	Edge& operator[](size_t index)
	{
		return m_edges[index];
	}

	inline constexpr size_t size() const
	{
		return m_edges.size();
	}
private:
	std::array<Edge, 4> m_edges;
};