#include "stdafx.h"
#include "VertexArray.h"

void createIndices(std::array<uint32_t, MAX_INDEX_COUNT>& indices, const uint32_t vertexCount);

template<typename VertexType>
VertexArray<VertexType>::VertexArray(uint32_t vertexCount)
	: m_vertexCount(vertexCount)
{
	glCreateVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glCreateBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, MAX_VERTEX_COUNT * sizeof(VertexType), nullptr, GL_DYNAMIC_DRAW);

    VertexType::createVertexAttribs(m_vao);
	createIndices(m_indices, vertexCount);

	glCreateBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indices), m_indices.data(), GL_STATIC_DRAW);

	std::cout << "creating vao: " << m_vao << "  " << &m_vao <<
		" vbo: " << m_vbo << "  " << &m_vbo <<
		" ibo: " << m_ibo << "  " << &m_ibo << '\n';
}

template<typename VertexType>
void VertexArray<VertexType>::clear()
{
	m_objectsCount = 0;
}

template<typename VertexType>
void VertexArray<VertexType>::addGeometryPiece(const VertexType* vertices)
{
	const uint32_t offset = m_objectsCount * m_vertexCount;
	memcpy(
		m_vertices.data() + offset,
		vertices, m_vertexCount * sizeof(VertexType)
	);
	m_objectsCount++;
}

template<typename VertexType>
void VertexArray<VertexType>::createSubData()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_objectsCount * m_vertexCount * sizeof(VertexType), m_vertices.data());
}

template<typename VertexType>
void VertexArray<VertexType>::onClean()
{
	std::cout << "deleting vao: " << m_vao << "  " << &m_vao <<
		" vbo: " << m_vbo << "  " << &m_vbo <<
		" ibo: " << m_ibo << "  " << &m_ibo << '\n';

	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ibo);
	glDeleteVertexArrays(1, &m_vao);
}

template<typename VertexType>
void VertexArray<VertexType>::bind() const
{
	glBindVertexArray(m_vao);
}

template<typename VertexType>
void VertexArray<VertexType>::unbind() const
{
	glBindVertexArray(0);
}

template<typename VertexType>
uint32_t VertexArray<VertexType>::getIndicesCount() const
{
	switch (m_vertexCount)
	{
	case 3:
		return 3;
	case 4:
		return 6;
	}
	return 0;
}


void createIndices(std::array<uint32_t, MAX_INDEX_COUNT>& indices, const uint32_t vertexCount)
{
	switch (vertexCount)
	{
		case 3:
		{
			uint32_t offset = 0;
			for (uint32_t i = 0; i < MAX_INDEX_COUNT; i += 3)
			{
				indices[i + 0] = 0 + offset;
				indices[i + 1] = 1 + offset;
				indices[i + 2] = 2 + offset;

				offset += vertexCount;
			}
			break;
		}
		case 4:
		{
			uint32_t offset = 0;
			for (uint32_t i = 0; i < MAX_INDEX_COUNT; i += 6)
			{
				indices[i + 0] = 0 + offset;
				indices[i + 1] = 1 + offset;
				indices[i + 2] = 2 + offset;

				indices[i + 3] = 2 + offset;
				indices[i + 4] = 3 + offset;
				indices[i + 5] = 0 + offset;

				offset += vertexCount;
			}
			break;
		}
		default:
			__debugbreak();
			break;
	}
}


///
/// Template specializations for different Vertex types
/// 

template class VertexArray<TextureVertex>;
template class VertexArray<ShadowVertex>;
template class VertexArray<UIVertex>;
template class VertexArray<ScreenVertex>;

void TextureVertex::createVertexAttribs(GLenum& vao)
{
	glEnableVertexArrayAttrib(vao, 0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), (const void*)offsetof(TextureVertex, center));
	
	glEnableVertexArrayAttrib(vao, 1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), (const void*)offsetof(TextureVertex, offset));

	glEnableVertexArrayAttrib(vao, 2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), (const void*)offsetof(TextureVertex, texCoords));

	glEnableVertexArrayAttrib(vao, 3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), (const void*)offsetof(TextureVertex, texID));

	glEnableVertexArrayAttrib(vao, 4);
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), (const void*)offsetof(TextureVertex, rotation));
}

void ShadowVertex::createVertexAttribs(GLenum& vao)
{
	glEnableVertexArrayAttrib(vao, 0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ShadowVertex), (const void*)offsetof(ShadowVertex, center));

	glEnableVertexArrayAttrib(vao, 1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ShadowVertex), (const void*)offsetof(ShadowVertex, offset));

	glEnableVertexArrayAttrib(vao, 2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ShadowVertex), (const void*)offsetof(ShadowVertex, texCoords));

	glEnableVertexArrayAttrib(vao, 3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(ShadowVertex), (const void*)offsetof(ShadowVertex, texID));

	glEnableVertexArrayAttrib(vao, 4);
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(ShadowVertex), (const void*)offsetof(ShadowVertex, rotation));

	glEnableVertexArrayAttrib(vao, 5);
	glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(ShadowVertex), (const void*)offsetof(ShadowVertex, lightIndex));

	glEnableVertexArrayAttrib(vao, 6);
	glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, sizeof(ShadowVertex), (const void*)offsetof(ShadowVertex, lightActive));
}

void UIVertex::createVertexAttribs(GLenum& vao)
{
	glEnableVertexArrayAttrib(vao, 0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (const void*)offsetof(UIVertex, offset));
	
	glEnableVertexArrayAttrib(vao, 1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (const void*)offsetof(UIVertex, texCoords));
	
	glEnableVertexArrayAttrib(vao, 2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (const void*)offsetof(UIVertex, texID));
}

void ScreenVertex::createVertexAttribs(GLenum& vao)
{
	glEnableVertexArrayAttrib(vao, 0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ScreenVertex), (const void*)offsetof(ScreenVertex, pos));

	glEnableVertexArrayAttrib(vao, 1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ScreenVertex), (const void*)offsetof(ScreenVertex, texCoords));
}
