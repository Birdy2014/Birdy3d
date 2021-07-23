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
GameObject* selected_object = nullptr;

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
    Application::defaultTheme = new Theme("#fbf1c7", "#282828", "#98971a", "#3c3836", "#1d2021", "#2e2b2a", "TTF/DejaVuSans.ttf", 20);

    Canvas canvas;
    Application::canvas = &canvas;

    canvas.add_child<FPSCounter>(0_px, Placement::TOP_RIGHT);

    Widget* menu = canvas.add_child<Widget>(0_px, 30_p, Placement::CENTER);
    menu->set_layout<DirectionalLayout>(DirectionalLayout::Direction::RIGHT, 10);
    menu->name = "menu";

    Button* closeButton = menu->add_child<Button>(0_px, Placement::BOTTOM_LEFT, "Close");
    closeButton->callback_click = [](InputClickEvent*) {
        glfwSetWindowShouldClose(Application::getWindow(), true);
    };

    Button* testButton = menu->add_child<Button>(0_px, Placement::BOTTOM_LEFT, "Fenster anzeigen", UIVector(200_px, 50_px));

    Textarea* area = menu->add_child<Textarea>(0_px, 100_px, Placement::BOTTOM_LEFT);
    area->weight = 2;
    area->append("Hallo Welt\nHallo Welt\naaaaaaaa\naaaaaaa\naaaaaa\naaaaaa");

    Window* treeWindow = canvas.add_child<Window>(0_px, UIVector(200_px, 300_px));
    treeWindow->set_layout<MaxLayout>();
    treeWindow->title("Scene");

    TreeView* tree = treeWindow->add_child<TreeView>(0_px, 100_p, Placement::TOP_LEFT);
    tree->callback_select = [&](TreeItem& item) {
        if (item.data.type() == typeid(GameObject*))
            selected_object = std::any_cast<GameObject*>(item.data);
    };

    Window* testWindow = canvas.add_child<Window>(0_px, 500_px);
    testWindow->set_layout<DirectionalLayout>(DirectionalLayout::Direction::DOWN, 10, true);
    testWindow->hidden = true;
    testWindow->title("Test");

    Textarea* area2 = testWindow->add_child<Textarea>(0_px, UIVector(100_p, 50_px), Placement::TOP_LEFT);
    area2->append("Dies ist ein Fenster");

    CheckBox* testCheckBox = testWindow->add_child<CheckBox>(UIVector(0_px, -50_px), Placement::TOP_LEFT, "Textures");

    NumberInput* inputR = testWindow->add_child<NumberInput>(UIVector(0_px, -80), UIVector(100_p, 25_px), Placement::TOP_LEFT, 0);
    NumberInput* inputG = testWindow->add_child<NumberInput>(UIVector(0_px, -110), UIVector(100_p, 25_px), Placement::TOP_LEFT, 0);
    NumberInput* inputB = testWindow->add_child<NumberInput>(UIVector(0_px, -140), UIVector(100_p, 25_px), Placement::TOP_LEFT, 0);
    inputR->min_value = 0;
    inputG->min_value = 0;
    inputB->min_value = 0;
    inputR->max_value = 1;
    inputG->max_value = 1;
    inputB->max_value = 1;
    inputR->value(1);
    inputG->value(1);
    inputB->value(1);

    NumberInput* inputX = testWindow->add_child<NumberInput>(UIVector(0_px, -170), UIVector(100_p, 25_px), Placement::TOP_LEFT, 0);
    NumberInput* inputY = testWindow->add_child<NumberInput>(UIVector(0_px, -200), UIVector(100_p, 25_px), Placement::TOP_LEFT, 0);
    NumberInput* inputZ = testWindow->add_child<NumberInput>(UIVector(0_px, -230), UIVector(100_p, 25_px), Placement::TOP_LEFT, 0);

    testButton->callback_click = [&testWindow](InputClickEvent*) {
        testWindow->hidden = !testWindow->hidden;
    };

    // GameObjects
    Scene* scene = new Scene("Scene");

    player = scene->add_child("Player", glm::vec3(0, 0, 3));
    player->add_component<Camera>(800, 600, true);
    player->add_component<FPPlayerController>();

    Material redTransparentMaterial;
    redTransparentMaterial.diffuse_color = glm::vec4(1.0f, 0.0f, 1.0f, 0.5f);
    Material blueTransparentMaterial;
    blueTransparentMaterial.diffuse_color = glm::vec4(0.0f, 1.0f, 1.0f, 0.5f);

    GameObject* obj = scene->add_child("obj", glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f));
    obj->add_component<ModelComponent>("./ressources/testObjects/cube.obj", &redTransparentMaterial);
    obj->add_component<Collider>(GenerationMode::COPY);

    GameObject* obj2 = scene->add_child("obj2", glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.0f), glm::vec3(10.0f, 1.0f, 10.0f));
    obj2->add_component<ModelComponent>("./ressources/testObjects/cube.obj", nullptr);

    GameObject* plane = scene->add_child("plane", glm::vec3(2.0f, -4.0f, 2.0f), glm::vec3(0.0f), glm::vec3(10.0f, 1.0f, 10.0f));
    plane->add_component<ModelComponent>("primitive::plane", nullptr);

    GameObject* obj3 = scene->add_child("obj3", glm::vec3(-3.0f, 5.0f, -1.0f), glm::vec3(0.0f));
    obj3->add_component<ModelComponent>("./ressources/testObjects/cube.obj", &blueTransparentMaterial);
    obj3->add_component<Collider>(GenerationMode::COPY);

    // Spheres
    GameObject* sphere1 = scene->add_child("Sphere1", glm::vec3(-3.0f, 1.0f, -1.0f), glm::vec3(0), glm::vec3(0.5));
    sphere1->add_component<ModelComponent>("./ressources/testObjects/sphere.obj", nullptr);
    sphere1->add_component<Collider>(GenerationMode::COPY);
    sphere1->add_component<TestComponent>();

    bool collision = false;
    Application::eventBus->subscribe<CollisionEvent>([&](CollisionEvent*) {
        collision = true;
    },
        sphere1);

    // Light
    GameObject* dirLight = scene->add_child("DirLight", glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(glm::radians(-45.0f), glm::radians(-45.0f), glm::radians(45.0f)));
    dirLight->add_component<DirectionalLight>(glm::vec3(0.2f), glm::vec3(0.7f));
    GameObject* pLight = scene->add_child("Point Light", glm::vec3(2.0f, 1.5f, 4.0f));
    pLight->add_component<PointLight>(glm::vec3(0.2f), glm::vec3(1.0f), 0.09f, 0.032f);
    GameObject* sLight = scene->add_child("Spotlight", glm::vec3(-6.0f, 3.0f, -2.0f), glm::vec3(glm::radians(-90.0f), 0, 0));
    sLight->add_component<Spotlight>(glm::vec3(0), glm::vec3(1.0f), glm::radians(40.0f), glm::radians(50.0f), 0.09f, 0.032f);

    GameObject* flashLight = player->add_child("Flashlight", glm::vec3(0), glm::vec3(0));
    flashLight->add_component<Spotlight>(glm::vec3(0), glm::vec3(1), glm::radians(30.0f), glm::radians(40.0f), 0.08f, 0.02f, false);
    flashLight->hidden = true;

    Application::eventBus->subscribe<InputKeyEvent>([&flashLight](InputKeyEvent*) {
        flashLight->hidden = !flashLight->hidden;
    },
        GLFW_KEY_F);

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
        newMaterial->diffuse_color = glm::vec4(random(0, 1), random(0, 1), random(0, 1), 1.0f);
        GameObject* newCube = scene->add_child("New Cube", glm::vec3(x, y, z));
        newCube->add_component<ModelComponent>("./ressources/testObjects/cube.obj", newMaterial);
        tree->sync_scene_tree(scene);
        Logger::debug("Created cube at x: ", x, " y: ", y, " z: ", z);
    },
        GLFW_KEY_N);

    scene->start();

    PointLight* light = pLight->getComponent<PointLight>();
    bool lightup = true;
    bool sphereup = true;

    tree->sync_scene_tree(scene);

    //Mainloop
    while (!glfwWindowShouldClose(Application::getWindow())) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        Application::deltaTime = deltaTime;

        Input::update();
        canvas.update();

        scene->update();

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
        player->getComponent<Camera>()->renderOutline(selected_object);

        if (collision) {
            Application::defaultTheme->text_renderer()->renderText("collision", 100, 0, 20, glm::vec4(1));
            collision = false;
        }

        canvas.draw();

        // swap Buffers
        glfwSwapBuffers(Application::getWindow());
        glfwPollEvents();
    }

    scene->cleanup();
    Application::cleanup();

    return 0;
}
