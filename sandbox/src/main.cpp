#define STB_IMAGE_IMPLEMENTATION
#include "Birdy3d.hpp"

GameObject *player;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main() {
	if (!Application::init("Birdy3d", 800, 600, "TTF/DejaVuSans.ttf", 30)) {
		return -1;
	}
	Input::init();

	// UI
	Widget canvas(glm::vec3(0.0f, 0.0f, 0.0f), Widget::Placement::CENTER);
	canvas.hidden = true;
	canvas.setOnClick([]() {
		std::cout << "Canvas clicked" << std::endl;
		return false;
	});
	canvas.addRectangle(glm::vec3(-100), glm::vec3(200), glm::vec4(1));
	Widget closeButton(glm::vec3(0), Widget::Placement::CENTER);
	closeButton.hidden = false;
	closeButton.setOnClick([]() {
    	glfwSetWindowShouldClose(Application::getWindow(), true);
		return true;
	});
	closeButton.addFilledRectangle(glm::vec2(-20.0, -20.0), glm::vec2(40.0, 40.0), glm::vec4(1.0f));
	closeButton.addRectangle(glm::vec2(-20.0, -20.0), glm::vec2(40.0, 40.0), glm::vec4(1, 0, 0, 1));
	closeButton.addText(glm::vec2(-20, -10), 20, "Quit", glm::vec4(0, 1, 1, 1));
	canvas.addChild(&closeButton);

	// GameObjects
	GameObject *scene = new GameObject();

	player = new GameObject(glm::vec3(0, 0, 3));
	player->addComponent(new Camera(800, 600, &canvas));
	player->addComponent(new FPPlayerController());
	scene->addChild(player);

	GameObject *obj = new GameObject(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f));
	obj->addComponent(new Model("./ressources/testObjects/cube.obj", false, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 16, glm::vec3(0.0f, 0.0f, 0.0f)));
	GameObject *obj2 = new GameObject(glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.0f), glm::vec3(10.0f, 1.0f, 10.0f));
	obj2->addComponent(new Model("./ressources/testObjects/cube.obj", false, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 16, glm::vec3(0.0f, 0.0f, 0.0f)));
	obj->addChild(obj2);
	scene->addChild(obj);

	// Light
	GameObject *dirLight = new GameObject(glm::vec3(0.0f, 3.0f, 0.0f));
	dirLight->addComponent(new DirectionalLight(glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(0.2f), glm::vec3(0.8f)));
	scene->addChild(dirLight);
	GameObject *pLight = new GameObject(glm::vec3(2.0f, 1.5f, 4.0f));
	pLight->addComponent(new PointLight(glm::vec3(0.2f), glm::vec3(1.0f), 0.09f, 0.032f));
	scene->addChild(pLight);

	scene->setScene();

	PointLight *light = scene->getComponents<PointLight>(true)[0];
	bool up = true;

	//Mainloop
	while(!glfwWindowShouldClose(Application::getWindow())) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		if (up) {
			light->object->pos.y += 0.1 * deltaTime;
			if (light->object->pos.y > 3)
				up = false;
		} else {
			light->object->pos.y -= 0.1 * deltaTime;
			if (light->object->pos.y < 1)
				up = true;
		}

		Input::update();
		canvas.updateEvents();

		scene->update(deltaTime);

		// draw the object
		player->getComponent<Camera>()->render();

		Application::getTextRenderer()->renderText("Hello World", 0, 0, 30, glm::vec4(1));

		// swap Buffers
    	glfwSwapBuffers(Application::getWindow());
    	glfwPollEvents();
	}

	scene->cleanup();

	glfwTerminate();
    return 0;
}