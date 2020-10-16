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
#include "objects/GameObject.hpp"

void createGBuffer(int width, int height);

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

// Gbuffer
unsigned int gBuffer, gPosition, gNormal, gAlbedoSpec, rboDepth;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
	aspectRatio = (float)width / (float)height;
	rebuildProjectionMatrix = true;
	glDeleteFramebuffers(1, &gBuffer);
	createGBuffer(width, height);
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

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad() {
    if (quadVAO == 0) {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

bool initGL() {
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
	glEnable(GL_CULL_FACE);

	// Set Viewport and resize callback
	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetWindowFocusCallback(window, window_focus_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	return true;
}

void createGBuffer(int width, int height) {
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	// - position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// - normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	// - color + specular color buffer
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

	// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
	unsigned int attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
	glDrawBuffers(3, attachments);

	// create and attach depth buffer (renderbuffer)
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int main() {
	if (!initGL()) {
		return -1;
	}

	// Shaders
	Shader geometryShader("./shaders/gBuffer.vert", "./shaders/gBuffer.frag");
	Shader lightShader("./shaders/lighting.vert", "./shaders/lighting.frag");
	Shader depthShader("./shaders/depth.vert", "./shaders/depth.frag");
	Shader uiShader("./shaders/ui.vert", "./shaders/ui.frag");

	createGBuffer(800, 600);

	// lightShader configuration
	lightShader.use();
	lightShader.setInt("gPosition", 0);
	lightShader.setInt("gNormal", 1);
	lightShader.setInt("gAlbedoSpec", 2);

	// GameObjects
	GameObject scene = GameObject(nullptr, &geometryShader, &depthShader);

	GameObject obj = GameObject(&scene, &geometryShader, &depthShader, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f));
	obj.addComponent(new Model(&obj, std::string(std::filesystem::current_path()) + std::string("/testObjects/cube.obj"), false, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 16, glm::vec3(0.0f, 0.0f, 0.0f)));
	GameObject obj2 = GameObject(&obj, &geometryShader, &depthShader, glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.0f), glm::vec3(10.0f, 1.0f, 10.0f));
	obj2.addComponent(new Model(&obj2, std::string(std::filesystem::current_path()) + std::string("/testObjects/cube.obj"), false, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 16, glm::vec3(0.0f, 0.0f, 0.0f)));
	obj.addChild(obj2);
	scene.addChild(obj);

	// Light
	GameObject dirLight = GameObject(&scene, &lightShader, &depthShader, glm::vec3(0.0f, 3.0f, 0.0f));
	dirLight.addComponent(new DirectionalLight(&dirLight, glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(0.2f), glm::vec3(0.8f), glm::vec3(1.0f)));
	scene.addChild(dirLight);
	GameObject pLight = GameObject(&scene, &lightShader, &depthShader, glm::vec3(2.0f, 1.5f, 4.0f));
	pLight.addComponent(new PointLight(&pLight, glm::vec3(0.2f), glm::vec3(1.0f), glm::vec3(1.0f), 0.09f, 0.032f));
	scene.addChild(pLight);
	
	lightShader.setFloat("material.shininess", 64);

	// UI
	widget.hidden = true;
	widget.setOnClick([]() {
    	glfwSetWindowShouldClose(window, true);
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

		scene.update(deltaTime);

		// draw the object

		// 1. geometry pass: render all geometric/color data to g-buffer
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		geometryShader.use();
		if (rebuildProjectionMatrix) {
			glm::mat4 projection = glm::perspective(glm::radians(80.0f), aspectRatio, 0.1f, 100.0f);
			geometryShader.setMat4("projection", projection);
			rebuildProjectionMatrix = false;
		}
		glm::mat4 view = cam.getViewMatrix();
		geometryShader.setMat4("view", view);
		glClearColor(0.0, 0.0, 0.0, 1.0); // keep it black so it doesn't leak into g-buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		for (Model *m : scene.getComponents<Model>(true)) {
			m->render();
		}

		lightShader.use();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
		std::vector<Light*> lights = scene.getComponents<Light>(true);
		int i;
		for (i = 0; i < lights.size(); i++) {
			lights[i]->use(&lightShader, i);
		}
		lightShader.setInt("nrLights", i);
		lightShader.setVec3("viewPos", cam.pos);
		renderQuad();

/*
		// render Depth map to quad for visual debugging
        // ---------------------------------------------
        debugShader.use();
        debugShader.setFloat("near_plane", 1.0f);
        debugShader.setFloat("far_plane", 7.5f);
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, depthMap);
        renderQuad();
*/
		// draw UI
		glClear(GL_DEPTH_BUFFER_BIT);
		widget.draw(uiShader);

		// swap Buffers
    	glfwSwapBuffers(window);
    	glfwPollEvents();
	}

	scene.cleanup();

	glfwTerminate();
    return 0;
}
