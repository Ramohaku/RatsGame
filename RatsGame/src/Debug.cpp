#include "stdafx.h"
#include "Debug.h"

#if _DEBUG
std::vector<Line> Line::s_debugLines;
#endif

#ifdef _DEBUG

namespace Debug {

	void printOpenGLMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		std::ostream& stream = *(std::ostream*)userParam;

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:
			stream << "OpenGL error: " << message << '\n';
			__debugbreak();
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			stream << "OpenGL warning: " << message << '\n';
			break;
		case GL_DEBUG_SEVERITY_LOW:
			stream << "OpenGL info: " << message << '\n';
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			//stream << "OpenGL notification: " << message << '\n';
			break;
		}
	}

	void enableOpenGLDebugging()
	{
		glDebugMessageCallback(printOpenGLMessage, (const void*)&std::cout);
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}

}

#endif _DEBUG