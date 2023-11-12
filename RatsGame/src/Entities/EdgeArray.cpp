#include "stdafx.h"
#include "EdgeArray.h"

EdgeArray::EdgeArray(const Vec2f& ratCenter, Sprite* sprite)
{
	const Vec2f center = sprite->getCenter();
	const Vec2f halfSize = sprite->getHalfSize();
	const float rotation = sprite->getRotation();

	const float s = sin(rotation);
	const float c = cos(rotation);
	const glm::mat2 rotMatrix(
		c, -s,
		s, c
	);

	m_edges[0].pos = center + rotMatrix * -halfSize;
	m_edges[1].pos = center + rotMatrix * Vec2f{ halfSize.x, -halfSize.y };
	m_edges[2].pos = center + rotMatrix * halfSize;
	m_edges[3].pos = center + rotMatrix * Vec2f{ -halfSize.x, halfSize.y };

	for (auto& edge : m_edges)
	{
		const float dx = abs(ratCenter.x - edge.pos.x);
		const float dy = abs(ratCenter.y - edge.pos.y);
		edge.dist = dx * dx + dy * dy;
	}
}

ptrdiff_t EdgeArray::getMinIndex() const
{
	return std::distance(m_edges.begin(), std::min_element(m_edges.begin(), m_edges.end(), [](const Edge& e1, const Edge& e2) {
		return e1.dist < e2.dist;
	}));
}
