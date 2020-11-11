#define STB_IMAGE_IMPLEMENTATION
#include "Birdy3d.hpp"

GameObject *player;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main() {
	if (!Application::init("Birdy3d", 800, 600)) {
		return -1;
	}
	Input::init();

	// Shaders
	Shader geometryShader("./shaders/gBuffer.vs", "./shaders/gBuffer.fs");
	Shader lightingShader("./shaders/lighting.vs", "./shaders/lighting.fs");
	Shader dirLightDepthShader("./shaders/dDepth.vs", "./shaders/dDepth.fs");
	Shader pointLightDepthShader("./shaders/pDepth.vs", "./shaders/pDepth.gs", "./shaders/pDepth.fs");
	Shader uiShader("./shaders/ui.vs", "./shaders/ui.fs");

	// lightShader configuration
	lightingShader.use();
	lightingShader.setInt("gPosition", 0);
	lightingShader.setInt("gNormal", 1);
	lightingShader.setInt("gAlbedoSpec", 2);

	// UI
	Widget widget(&uiShader, glm::vec3(0.0f, 0.0f, 0.0f), Widget::Placement::CENTER);
	widget.hidden = true;
	widget.setOnClick([]() {
    	glfwSetWindowShouldClose(Application::getWindow(), true);
	});
	uiShader.use();
	widget.addFilledRectangle(glm::vec2(0.0, 0.0), glm::vec2(40.0, 40.0), glm::vec3(1.0f), 0, 0.5);
	widget.addRectangle(glm::vec2(0.0, 0.0), glm::vec2(40.0, 40.0), glm::vec3(1, 0, 0));
	widget.fillBuffer();

	// GameObjects
	GameObject *scene = new GameObject(&geometryShader);

	player = new GameObject(&geometryShader, glm::vec3(0, 0, 3));
	player->addComponent(new Camera(&lightingShader, 800, 600, &widget));
	player->addComponent(new FPPlayerController());
	scene->addChild(player);

	GameObject *obj = new GameObject(&geometryShader, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f));
	obj->addComponent(new Model("./ressources/testObjects/cube.obj", false, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 16, glm::vec3(0.0f, 0.0f, 0.0f)));
	GameObject *obj2 = new GameObject(&geometryShader, glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.0f), glm::vec3(10.0f, 1.0f, 10.0f));
	obj2->addComponent(new Model("./ressources/testObjects/cube.obj", false, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 16, glm::vec3(0.0f, 0.0f, 0.0f)));
	obj->addChild(obj2);
	scene->addChild(obj);

	// Light
	GameObject *dirLight = new GameObject(&geometryShader, glm::vec3(0.0f, 3.0f, 0.0f));
	dirLight->addComponent(new DirectionalLight(&dirLightDepthShader, glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(0.2f), glm::vec3(0.8f)));
	scene->addChild(dirLight);
	GameObject *pLight = new GameObject(&geometryShader, glm::vec3(2.0f, 1.5f, 4.0f));
	pLight->addComponent(new PointLight(&pointLightDepthShader, glm::vec3(0.2f), glm::vec3(1.0f), 0.09f, 0.032f));
	scene->addChild(pLight);

	scene->setScene();

	//Mainloop
	while(!glfwWindowShouldClose(Application::getWindow())) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		Input::update();
		widget.updateEvents();

		scene->update(deltaTime);

		// draw the object
		player->getComponent<Camera>()->render();

		// swap Buffers
    	glfwSwapBuffers(Application::getWindow());
    	glfwPollEvents();
	}

	scene->cleanup();

	glfwTerminate();
    return 0;
}
