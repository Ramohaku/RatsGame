#include "stdafx.h"
#include "Shader.h"

Shader::Shader(const std::string& filepathVertex, const std::string& filepathFragment)
{
	std::string vertexSource = loadFromFile(filepathVertex);
	std::string fragmentSource = loadFromFile(filepathFragment);

	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

	unsigned int program = glCreateProgram();

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	glValidateProgram(program);

	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
	if (isLinked == GL_FALSE)
	{
		int length;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetProgramInfoLog(program, length, &length, message);
		std::cout << "Failed to link program!\n";
		std::cout << message << '\n';
		glDeleteProgram(program);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	m_rendererID = program;
}

Shader::~Shader()
{
	glDeleteProgram(m_rendererID);
}

void Shader::bind() const
{
	glUseProgram(m_rendererID);
}

void Shader::unbind() const
{
	glUseProgram(0);
}

void Shader::setUniform1i(const std::string& name, int value)
{
	glUniform1i(getUniformLocation(name), value);
}

void Shader::setUniform1iv(const std::string& name, uint32_t size, const int* values)
{
	glUniform1iv(getUniformLocation(name), size, values);
}

void Shader::setUniform1f(const std::string& name, float value)
{
	glUniform1f(getUniformLocation(name), value);
}

void Shader::setUniform1fv(const std::string& name, uint32_t size, const float* values)
{
	glUniform1fv(getUniformLocation(name), size, values);
}

void Shader::setUniform1bv(const std::string& name, uint32_t size, const bool* values)
{
	glUniform1iv(getUniformLocation(name), size, (const int*)values);
}

void Shader::setUniform2f(const std::string& name, const Vec2f& value)
{
	glUniform2f(getUniformLocation(name), value.x, value.y);
}

void Shader::setUniform3f(const std::string& name, const Vec3f& value)
{
	glUniform3f(getUniformLocation(name), value.x, value.y, value.z);
}

void Shader::setUniform3fv(const std::string& name, size_t size, const Vec3f* values)
{
	glUniform3fv(getUniformLocation(name), size, (GLfloat*)values);
}

void Shader::setUniform4f(const std::string& name, const Vec4f& value)
{
	glUniform4f(getUniformLocation(name), value.x, value.y, value.z, value.w);
}

void Shader::setUniform4fv(const std::string& name, size_t size, const Vec4f* values)
{
	glUniform4fv(getUniformLocation(name), size, (GLfloat*)values);
}

void Shader::setUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

GLuint Shader::compileShader(GLenum type, const std::string& source)
{
	GLuint shader = glCreateShader(type);

	const GLchar* sourceCStr = source.c_str();
	glShaderSource(shader, 1, &sourceCStr, 0);

	glCompileShader(shader);

	int result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(shader, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!\n";
		std::cout << message << '\n';
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

std::string Shader::loadFromFile(const std::string& filepath)
{
	std::ifstream stream(filepath);

	std::string source;
	std::string line;
	while (getline(stream, line))
		source += line + '\n';

	stream.close();

	return source;
}

GLint Shader::getUniformLocation(const std::string& name) const
{
	if (m_uniformLocationCache.find(name) != m_uniformLocationCache.end())
		return m_uniformLocationCache.at(name);

	GLint location = glGetUniformLocation(m_rendererID, name.c_str());
	if (location == -1)
		std::cout << "Warning: Uniform \"" << name << "\" doesn't exist!\n";
	m_uniformLocationCache[name] = location;
	return location;
}
