#include "core/Application.hpp"

#include "core/Input.hpp"
#include "core/Logger.hpp"
#include "core/RessourceManager.hpp"

GLFWwindow *Application::window = nullptr;
EventDispatcher<Application::EventArg> *Application::eventDispatcher = nullptr;
TextRenderer *Application::textRenderer = nullptr;

bool Application::init(const char *windowName, int width, int height, const std::string &font, unsigned int fontSize) {
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create Window
	Application::window = glfwCreateWindow(width, height, windowName, nullptr, nullptr);
	if (window == nullptr) {
    	Logger::error("Failed to create GLFW window");
    	glfwTerminate();
    	return false;
	}
	glfwMakeContextCurrent(window);

	// Load OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    	Logger::error("Failed to initialize GLAD");
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

	// Init variables
	eventDispatcher = new EventDispatcher<Application::EventArg>();
	textRenderer = new TextRenderer(RessourceManager::getFontPath(font), fontSize);

	return true;
}

void Application::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
	eventDispatcher->emitEvent(Application::EVENT_FRAMEBUFFER_SIZE, { width, height });
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

void *Application::registerEvent(Application::EventTypes type, std::function<void(Application::EventArg)> callback) {
	eventDispatcher->addHandler(type, callback);
}

glm::vec2 Application::getViewportSize() {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    return glm::vec2(viewport[2], viewport[3]);
}

TextRenderer *Application::getTextRenderer() {
	return textRenderer;
}
