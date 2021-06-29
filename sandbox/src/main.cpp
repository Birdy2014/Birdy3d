#include "Birdy3d.hpp"
#include <cstdlib>
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace Birdy3d;

class TestComponent : public Component {
public:
    void start() override {
        Application::eventBus->subscribe(this, &TestComponent::onCollision);
    }

    void cleanup() override {
        Application::eventBus->unsubscribe(this, &TestComponent::onCollision);
    }

    void onCollision(CollisionEvent* event) {
        switch (event->type) {
        case CollisionEvent::ENTER:
            Logger::debug("ENTER");
            break;
        case CollisionEvent::COLLIDING:
            break;
        case CollisionEvent::EXIT:
            Logger::debug("EXIT");
            break;
        }
    }
};

GameObject* player;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void handler(int sig) {
    void* array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}

int main() {
    signal(SIGSEGV, handler);

    if (!Application::init("Birdy3d", 800, 600)) {
        return -1;
    }
    Input::init();

    // UI
    Application::defaultTheme = new Theme("#fbf1c7", "#282828", "#98971a", "TTF/DejaVuSans.ttf", 22);

    Canvas canvas;

    FPSCounter counter(0_px, Placement::TOP_RIGHT);
    canvas.addChild(&counter);

    DirectionalLayout menu(DirectionalLayout::Direction::RIGHT, Placement::CENTER, 30_p, 10);
    menu.name = "menu";
    canvas.addChild(&menu);

    Button closeButton(0_px, Placement::BOTTOM_LEFT, "Close", 20);
    closeButton.clickCallback = [](InputClickEvent*) {
        glfwSetWindowShouldClose(Application::getWindow(), true);
    };
    menu.addChild(&closeButton);

    Button testButton(0_px, Placement::BOTTOM_LEFT, "TestButton", 20, UIVector(200_px, 50_px));
    menu.addChild(&testButton);

    Textarea area(0_px, 100_px, Placement::BOTTOM_LEFT);
    area.weight = 2;
    area.append("Hallo Welt\nHallo Welt\naaaaaaaa\naaaaaaa\naaaaaa\naaaaaa");
    menu.addChild(&area);

    Window testWindow(0_px, 500_px);
    testWindow.hidden = true;
    canvas.addChild(&testWindow);

    AbsoluteLayout winLayout(0_px, 100_p);
    testWindow.child = &winLayout;

    Textarea area2(0_px, UIVector(100_p, 50_px), Placement::TOP_LEFT);
    area2.append("Dies ist ein Fenster");
    winLayout.addChild(&area2);

    CheckBox testCheckBox(UIVector(0_px, -50_px), Placement::TOP_LEFT, "Test");
    winLayout.addChild(&testCheckBox);

    testButton.clickCallback = [&testWindow](InputClickEvent*) {
        testWindow.hidden = !testWindow.hidden;
    };

    // GameObjects
    Scene* scene = new Scene();

    player = new GameObject(glm::vec3(0, 0, 3));
    player->addComponent(new Camera(800, 600, true, &canvas));
    player->addComponent(new FPPlayerController());
    scene->addChild(player);

    Material defaultMaterial;
    Material redTransparentMaterial;
    redTransparentMaterial.diffuse_color(glm::vec4(1.0f, 0.0f, 1.0f, 0.5f));
    Material blueTransparentMaterial;
    blueTransparentMaterial.diffuse_color(glm::vec4(0.0f, 1.0f, 1.0f, 0.5f));

    GameObject* obj = new GameObject(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f));
    obj->addComponent(new ModelComponent("./ressources/testObjects/cube.obj", &redTransparentMaterial));
    obj->addComponent(new Collider());
    scene->addChild(obj);

    GameObject* obj2 = new GameObject(glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.0f), glm::vec3(10.0f, 1.0f, 10.0f));
    obj2->addComponent(new ModelComponent("./ressources/testObjects/cube.obj", &defaultMaterial));
    scene->addChild(obj2);

    GameObject* obj3 = new GameObject(glm::vec3(-3.0f, 5.0f, -1.0f), glm::vec3(0.0f));
    obj3->addComponent(new ModelComponent("./ressources/testObjects/cube.obj", &blueTransparentMaterial));
    obj3->addComponent(new Collider());
    scene->addChild(obj3);

    // Spheres
    GameObject* sphere1 = new GameObject(glm::vec3(-3.0f, 1.0f, -1.0f), glm::vec3(0), glm::vec3(0.5));
    sphere1->addComponent(new ModelComponent("./ressources/testObjects/sphere.obj", &defaultMaterial));
    sphere1->addComponent(new Collider(new CollisionSphere(glm::vec3(0), 1)));
    sphere1->addComponent(new TestComponent());
    scene->addChild(sphere1);

    bool collision = false;
    Application::eventBus->subscribe<CollisionEvent>([&](CollisionEvent*) {
        collision = true;
    },
        sphere1, CollisionEvent::COLLIDING);

    // Light
    GameObject* dirLight = new GameObject(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(glm::radians(-45.0f), glm::radians(-45.0f), glm::radians(45.0f)));
    dirLight->addComponent(new DirectionalLight(glm::vec3(0.2f), glm::vec3(0.7f)));
    scene->addChild(dirLight);
    GameObject* pLight = new GameObject(glm::vec3(2.0f, 1.5f, 4.0f));
    pLight->addComponent(new PointLight(glm::vec3(0.2f), glm::vec3(1.0f), 0.09f, 0.032f));
    scene->addChild(pLight);
    GameObject* sLight = new GameObject(glm::vec3(-6.0f, 3.0f, -2.0f), glm::vec3(glm::radians(-90.0f), 0, 0));
    sLight->addComponent(new Spotlight(glm::vec3(0), glm::vec3(1.0f), glm::radians(40.0f), glm::radians(50.0f), 0.09f, 0.032f));
    scene->addChild(sLight);

    Application::eventBus->subscribe<InputKeyEvent>([&](InputKeyEvent*) {
        pLight->hidden = !pLight->hidden;
    },
        GLFW_KEY_L);

    Application::eventBus->subscribe<InputKeyEvent>([&](InputKeyEvent*) {
        auto random = [](float min, float max) {
            float zeroToOne = ((float)std::rand() / (float)RAND_MAX);
            return zeroToOne * (std::abs(min) + std::abs(max)) + min;
        };
        float x = random(-30, 30);
        float y = random(-30, 30);
        float z = random(-30, 30);
        Material* newMaterial = new Material();
        newMaterial->diffuse_color(glm::vec4(random(0, 1), random(0, 1), random(0, 1), 1.0f));
        GameObject* newCube = new GameObject(glm::vec3(x, y, z));
        newCube->addComponent(new ModelComponent("./ressources/testObjects/cube.obj", newMaterial));
        scene->addChild(newCube);
        Logger::debug("Created cube at x: ", x, " y: ", y, " z: ", z);
    },
        GLFW_KEY_N);

    scene->start();
    PhysicsWorld* physicsWorld = new PhysicsWorld(scene);

    PointLight* light = pLight->getComponent<PointLight>();
    bool lightup = true;
    bool sphereup = true;

    //Mainloop
    while (!glfwWindowShouldClose(Application::getWindow())) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        Application::deltaTime = deltaTime;

        Input::update();
        canvas.update();

        scene->update();
        physicsWorld->update();

        Application::eventBus->flush();

        if (lightup) {
            light->object->transform.position.y += 0.1 * deltaTime;
            if (light->object->transform.position.y > 3)
                lightup = false;
        } else {
            light->object->transform.position.y -= 0.1 * deltaTime;
            if (light->object->transform.position.y < 1)
                lightup = true;
        }

        if (sphereup) {
            sphere1->transform.position.y += 0.4 * deltaTime;
            if (sphere1->transform.position.y > 5)
                sphereup = false;
        } else {
            sphere1->transform.position.y -= 0.4 * deltaTime;
            if (sphere1->transform.position.y < 1)
                sphereup = true;
        }

        // draw the object
        player->getComponent<Camera>()->render();

        if (collision) {
            Application::getTextRenderer()->renderText("collision", 100, 0, 20, glm::vec4(1));
            collision = false;
        }

        // swap Buffers
        glfwSwapBuffers(Application::getWindow());
        glfwPollEvents();
    }

    scene->cleanup();
    Application::cleanup();

    return 0;
}
