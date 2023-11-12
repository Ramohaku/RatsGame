#pragma once
#include "Utils.h"

class Shader
{
public:
	Shader(const std::string& filepathVertex, const std::string& filepathFragment);
	~Shader();

	void bind() const;
	void unbind() const;

	void setUniform1i(const std::string& name, int value);
	void setUniform1iv(const std::string& name, uint32_t size, const int* values);
	void setUniform1f(const std::string& name, float value);
	void setUniform1fv(const std::string& name, uint32_t size, const float* values);
	void setUniform1bv(const std::string& name, uint32_t size, const bool* values);
	void setUniform2f(const std::string& name, const Vec2f& value);
	void setUniform3f(const std::string& name, const Vec3f& value);
	void setUniform3fv(const std::string& name, size_t size, const Vec3f* values);
	void setUniform4f(const std::string& name, const Vec4f& value);
	void setUniform4fv(const std::string& name, size_t size, const Vec4f* values);
	void setUniformMat4f(const std::string& name, const glm::mat4& matrix);

	inline GLuint getRendererID() const { return m_rendererID; }
private:
	GLuint compileShader(GLenum type, const std::string& source);
	std::string loadFromFile(const std::string& filepath);
	GLint getUniformLocation(const std::string& name) const;
private:
	GLenum m_rendererID = 0;
	mutable std::unordered_map<std::string, GLint> m_uniformLocationCache;
};

