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
		Logger::debug("Canvas clicked");
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
	player->addComponent(new Camera(800, 600, true, &canvas));
	player->addComponent(new FPPlayerController());
	scene->addChild(player);

	GameObject *obj = new GameObject(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f));
	obj->addComponent(new Model("./ressources/testObjects/cube.obj", false, glm::vec4(1.0f, 0.0f, 1.0f, 0.5f), 1, glm::vec3(0.0f, 0.0f, 0.0f)));
	obj->addComponent(new Collider());
	scene->addChild(obj);

	GameObject *obj2 = new GameObject(glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.0f), glm::vec3(10.0f, 1.0f, 10.0f));
	obj2->addComponent(new Model("./ressources/testObjects/cube.obj", false, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 1, glm::vec3(0.0f, 0.0f, 0.0f)));
	scene->addChild(obj2);

	GameObject *obj3 = new GameObject(glm::vec3(-3.0f, 5.0f, -1.0f), glm::vec3(0.0f));
	obj3->addComponent(new Model("./ressources/testObjects/cube.obj", false, glm::vec4(0.0f, 1.0f, 1.0f, 0.5f), 1, glm::vec3(0.0f, 0.0f, 0.0f)));
	obj3->addComponent(new Collider());
	scene->addChild(obj3);

	// Spheres
	GameObject *sphere1 = new GameObject(glm::vec3(-3.0f, 1.0f, -1.0f), glm::vec3(0), glm::vec3(0.5));
	sphere1->addComponent(new Model("./ressources/testObjects/sphere.obj", false, glm::vec4(1)));
	sphere1->addComponent(new Collider(new CollisionSphere(glm::vec3(0), 1)));
	scene->addChild(sphere1);

	bool collision = false;
	sphere1->getComponent<Collider>()->eventDispatcher->addHandler(Collider::COLLISION, [&](Collider::EventArg arg) {
		collision = true;
	});

	// Light
	GameObject *dirLight = new GameObject(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(glm::radians(-45.0f), glm::radians(-45.0f), glm::radians(45.0f)));
	dirLight->addComponent(new DirectionalLight(glm::vec3(0.2f), glm::vec3(0.7f)));
	scene->addChild(dirLight);
	GameObject *pLight = new GameObject(glm::vec3(2.0f, 1.5f, 4.0f));
	pLight->addComponent(new PointLight(glm::vec3(0.2f), glm::vec3(1.0f), 0.09f, 0.032f));
	scene->addChild(pLight);
	GameObject *sLight = new GameObject(glm::vec3(-6.0f, 3.0f, -2.0f), glm::vec3(glm::radians(-90.0f), 0, 0));
	sLight->addComponent(new Spotlight(glm::vec3(0), glm::vec3(1.0f), glm::radians(40.0f), glm::radians(50.0f), 0.09f, 0.032f));
	scene->addChild(sLight);

	scene->setScene();
	scene->start();

	PointLight *light = pLight->getComponent<PointLight>();
	bool lightup = true;
	bool sphereup = true;

	//Mainloop
	while(!glfwWindowShouldClose(Application::getWindow())) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		Input::update();
		canvas.updateEvents();

		scene->update(deltaTime);

		if (lightup) {
			light->object->transform.position.y += 0.1 * deltaTime;
			if (light->object->transform.position.y > 3)
				lightup = false;
		} else {
			light->object->transform.position.y -= 0.1 * deltaTime;
			if (light->object->transform.position.y < 1)
				lightup = true;
		}

		if (!collision) {
			if (sphereup) {
				sphere1->transform.position.y += 0.4 * deltaTime;
				if (sphere1->transform.position.y > 5)
					sphereup = false;
			} else {
				sphere1->transform.position.y -= 0.4 * deltaTime;
				if (sphere1->transform.position.y < 1)
					sphereup = true;
			}
		}

		// draw the object
		player->getComponent<Camera>()->render();

		Application::getTextRenderer()->renderText("FPS: " + std::to_string((int)round(1 / deltaTime)), 0, 0, 10, glm::vec4(1));
		if (collision) {
			Application::getTextRenderer()->renderText("collision", 100, 0, 20, glm::vec4(1));
			collision = false;
		}

		// swap Buffers
    	glfwSwapBuffers(Application::getWindow());
    	glfwPollEvents();
	}

	scene->cleanup();

	glfwTerminate();
    return 0;
}
