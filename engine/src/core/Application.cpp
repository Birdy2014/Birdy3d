#include "core/Application.hpp"

#include <iostream>
#include "core/Input.hpp"

GLFWwindow *Application::window = nullptr;
EventHandler<framebufferSizeArg> *Application::framebufferSizeEventHandler = nullptr;

bool Application::init(const char *windowName, int width, int height) {
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create Window
	Application::window = glfwCreateWindow(width, height, windowName, nullptr, nullptr);
	if (window == nullptr) {
    	std::cout << "Failed to create GLFW window" << std::endl;
    	glfwTerminate();
    	return false;
	}
	glfwMakeContextCurrent(window);

	// Load OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    	std::cout << "Failed to initialize GLAD" << std::endl;
    	return false;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Set Viewport and resize callback
	glViewport(0, 0, width, height);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetWindowFocusCallback(window, window_focus_callback);
	framebufferSizeEventHandler = new EventHandler<framebufferSizeArg>();

	return true;
}

void Application::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
	framebufferSizeEventHandler->emitEvent({ width, height });
}

void Application::window_focus_callback(GLFWwindow *window, int focused) {
	if (focused && Input::isCursorHidden()) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	} else if (focused) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	} else {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

GLFWwindow *Application::getWindow() {
	return window;
}

EventHandler<framebufferSizeArg> *Application::getFramebufferSizeEventHandler() {
	return framebufferSizeEventHandler;
}

glm::vec2 Application::getViewportSize() {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    return glm::vec2(viewport[2], viewport[3]);
}
