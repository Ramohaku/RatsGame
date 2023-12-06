#pragma once

#include "Utils.h"

#define ENABLE_ANTIALIASING 1

class Window
{
public:
	Window(int width, int height, const char* title, float scale);
	Window(const char* title, float scale, bool fullscreen);
	Window(const Window&) = delete;
	Window(Window&&) = delete;
	~Window();

	bool isOpen() const;
	void clear(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f);
	void display();

	void setCenter(const Vec2f& center);
	inline void setScale(float scale) { m_scale = scale; }

	inline GLFWwindow* getGlfwWindow() const { return m_glfwWindow; }
	inline float getScale() const { return m_scale; }
	inline const glm::mat4& getProj() const { return m_proj; }
	inline const glm::mat4& getView() const { return m_view; }
	inline int getWidth() const { return m_width; }
	inline int getHeight() const { return m_height; }
private:
	void initWindow();
	void createWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share);
private:
	GLFWwindow* m_glfwWindow = nullptr;

	int m_width, m_height;

	float m_scale = 1.0f;
	glm::mat4 m_proj;
	glm::mat4 m_view;
};

