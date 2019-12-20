#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "render/Shader.hpp"
#include "render/Texture.hpp"
#include "render/Camera.hpp"
#include "render/Model.hpp"
#include "render/DirectionalLight.hpp"
#include "render/PointLight.hpp"
#include "ui/Widget.hpp"

GLFWwindow *window;

Camera cam(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);

Widget widget(glm::vec3(0.0f, 0.0f, 0.0f), CENTER);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float aspectRatio = 1;
bool rebuildProjectionMatrix = true;

float lastX = 400, lastY = 300;
bool firstMouse = true;

bool hideMouse = true;
bool updatedHiddenStatus = false;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
	aspectRatio = (float)width / (float)height;
	rebuildProjectionMatrix = true;
}

void window_focus_callback(GLFWwindow *window, int focused) {
	if (focused) {
		if (hideMouse)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		
	} else {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		if (!updatedHiddenStatus) {
			hideMouse = !hideMouse;
			widget.hidden = !widget.hidden;
			updatedHiddenStatus = true;
			window_focus_callback(window, true);
		}
	} else {
		updatedHiddenStatus = false;
	}

	float cameraSpeed = 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cam.pos += cameraSpeed * cam.front;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cam.pos -= cameraSpeed * cam.front;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cam.pos -= glm::normalize(glm::cross(cam.front, cam.up)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cam.pos += glm::normalize(glm::cross(cam.front, cam.up)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		cam.pos += glm::vec3(0.0f, cameraSpeed, 0.0f);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		cam.pos += glm::vec3(0.0f, -cameraSpeed, 0.0f);

	cam.updateCameraVectors();
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (!hideMouse) {
		return;
	}

	if(firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	cam.yaw   += xoffset;
	cam.pitch += yoffset;

	if(cam.pitch > 89.0f)
  		cam.pitch =  89.0f;
	if(cam.pitch < -89.0f)
  		cam.pitch = -89.0f;

	cam.updateCameraVectors();
}


bool initGL() {
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 16); // 16xMSAA

	// Create Window
	window = glfwCreateWindow(800, 600, "Birdy3d", nullptr, nullptr);
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
	glEnable(GL_MULTISAMPLE);

	// Set Viewport and resize callback
	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetWindowFocusCallback(window, window_focus_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	return true;
}

int main() {
	if (!initGL()) {
		return -1;
	}

	Model obj(std::string(std::filesystem::current_path()) + std::string("/testObjects/cube.obj"), glm::vec3(0.0f, 1.0f, -1.0f), glm::vec3(1.0f), false, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	// Shaders
	Shader textureShader("./shaders/lighting.vert", "./shaders/lighting.frag");
	Shader uiShader("./shaders/ui.vert", "./shaders/ui.frag");
	textureShader.use();

	// Light
	DirectionalLight dirLight(glm::vec3(0.9f, -1.0f, 0.8f), glm::vec3(0.2f), glm::vec3(0.8f), glm::vec3(1.0f));
	PointLight pLight(glm::vec3(2.0f, 1.5f, 4.0f), glm::vec3(0.2f), glm::vec3(1.0f), glm::vec3(1.0f), 0.09f, 0.032f);
	pLight.use(textureShader, 0);
	dirLight.use(textureShader, 1);
	
	textureShader.setFloat("material.shininess", 64);

	// UI
	widget.hidden = true;
	widget.setOnClick([]() {
    	glfwSetWindowShouldClose(window, true);
		std::cout << "Clicked!" << std::endl;
	});
	uiShader.use();
	widget.addRectangle(glm::vec2(0.0, 0.0), glm::vec2(40.0, 40.0), glm::vec3(1.0f));
	widget.fillBuffer();

	//Mainloop
	while(!glfwWindowShouldClose(window)) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);
		widget.updateEvents(window);
		textureShader.use();

		if (rebuildProjectionMatrix) {
			glm::mat4 projection = glm::perspective(glm::radians(80.0f), aspectRatio, 0.1f, 100.0f);
			textureShader.setMat4("projection", projection);
			rebuildProjectionMatrix = false;
		}

		// clear screen
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// draw the object
		glm::mat4 view = cam.getViewMatrix();
		textureShader.setMat4("view", view);
		textureShader.setVec3("viewPos", cam.pos);
		obj.draw(textureShader);

		// draw UI
		glClear(GL_DEPTH_BUFFER_BIT);
		widget.draw(uiShader);

		// swap Buffers
    	glfwSwapBuffers(window);
    	glfwPollEvents();
	}

	obj.cleanup();

	glfwTerminate();
    return 0;
}
