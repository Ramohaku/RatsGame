#include "stdafx.h"
#include "Debug.h"
#include "Window.h"

#if ENABLE_ANTIALIASING
static constexpr int SAMPLES = 8;
#endif

Window::Window(int width, int height, const char* title, float scale)
    : m_width(width), m_height(height), m_scale(scale),
    m_proj(glm::ortho(0.0f,
        static_cast<float>(width) * scale, 0.0f,
        static_cast<float>(height) * scale, -1.0f, 1.0f
    )),
    m_view(glm::translate(glm::mat4(1.0f),
        glm::vec3(
            static_cast<float>(width) * scale / 2.0f,
            static_cast<float>(height) * scale / 2.0f,
    0.0f)))
{
    initWindow();
    createWindow(width, height, title, nullptr, nullptr);
}

Window::Window(const char* title, float scale, bool fullscreen)
    : m_scale(scale)
{
    initWindow();

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();

    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    m_width = mode->width;
    m_height = mode->height;

    m_proj = glm::ortho(0.0f,
        static_cast<float>(m_width) * scale, 0.0f,
        static_cast<float>(m_height) * scale, -1.0f, 1.0f
    );
    m_view = glm::translate(glm::mat4(1.0f), glm::vec3(
        static_cast<float>(m_width) * scale / 2.0f,
        static_cast<float>(m_height) * scale / 2.0f,
    0.0f));

    createWindow(m_width, m_height, title, fullscreen ? monitor : nullptr, nullptr);
}

void Window::initWindow()
{
    if (!glfwInit())
    {
        ASSERT(false);
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if ENABLE_ANTIALIASING
    glfwWindowHint(GLFW_SAMPLES, SAMPLES);
#endif
}

void Window::createWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
{
    m_glfwWindow = glfwCreateWindow(width, height, title, monitor, share);

    if (!m_glfwWindow)
    {
        glfwTerminate();
        ASSERT(false);
        return;
    }

    glfwMakeContextCurrent(m_glfwWindow);
    glfwSwapInterval(1);
}

Window::~Window()
{
    glfwDestroyWindow(m_glfwWindow);
    glfwTerminate();
}

bool Window::isOpen() const
{
    return !glfwWindowShouldClose(m_glfwWindow);
}

void Window::clear(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::display()
{
    glfwSwapBuffers(m_glfwWindow);
}

void Window::setCenter(const Vec2f& center)
{
    m_view = glm::translate(glm::mat4(1.0f), glm::vec3(
        static_cast<float>(m_width) * m_scale / 2.0f - center.x,
        static_cast<float>(m_height) * m_scale / 2.0f - center.y,
    0.0f));
}

void Window::setScale(float scale)
{
    m_scale = scale;
    m_proj = glm::ortho(0.0f,
        static_cast<float>(m_width) * scale, 0.0f,
        static_cast<float>(m_height) * scale, -1.0f, 1.0f
    );
}
