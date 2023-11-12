#pragma once
#include "Utils.h"

#define MAX_VERTEX_COUNT 4 * 1000
#define MAX_INDEX_COUNT 6 * 1000

struct TextureVertex
{
    Vec2f center;
    Vec2f offset;
    Vec2f texCoords;
    float texID;
    float rotation;

    static void createVertexAttribs(GLenum& vao);
};

struct ShadowVertex
{
    Vec2f center;
    Vec2f offset;
    Vec2f texCoords;
    float texID;
    float rotation;
    float lightIndex;
    float lightActive;

    static void createVertexAttribs(GLenum& vao);
};

struct UIVertex
{
    Vec2f offset;
    Vec2f texCoords;
    float texID;

    static void createVertexAttribs(GLenum& vao);
};

template<typename VertexType>
class VertexArray
{
public:
    VertexArray(uint32_t vertexCount);
    ~VertexArray();

    void clear();
    void addGeometryPiece(const VertexType* vertices);
    void createSubData();

    void bind() const;
    void unbind() const;

    inline const uint32_t getVertexCount() const { return m_vertexCount; }
    inline uint32_t getObjectsCount() const { return m_objectsCount; }
    uint32_t getIndicesCount() const;
private:
    GLenum m_vao = 0;
    GLenum m_vbo = 0;
    GLenum m_ibo = 0;

    std::array<VertexType, MAX_VERTEX_COUNT> m_vertices;
    std::array<uint32_t, MAX_INDEX_COUNT> m_indices;
    const uint32_t m_vertexCount; // number of vertices in one piece of geometry
    uint32_t m_objectsCount;
};
