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
        Application::event_bus->subscribe(this, &TestComponent::onCollision);
    }

    void cleanup() override {
        Application::event_bus->unsubscribe(this, &TestComponent::onCollision);
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

class MoveUpDown : public Component {
public:
    MoveUpDown(float speed, float limit_down, float limit_up)
        : m_speed(speed)
        , m_limit_down(limit_down)
        , m_limit_up(limit_up) { }

    void update() override {
        if (m_up) {
            object->transform.position.y += m_speed * Application::delta_time;
            if (object->transform.position.y > m_limit_up)
                m_up = false;
        } else {
            object->transform.position.y -= m_speed * Application::delta_time;
            if (object->transform.position.y < m_limit_down)
                m_up = true;
        }

    }

private:
    bool m_up = true;
    float m_speed;
    float m_limit_down;
    float m_limit_up;
};

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
    Application::theme = new Theme("#fbf1c7", "#282828", "#98971a", "#3c3836", "#1d2021", "#ffffff11", "#0000a050", "TTF/DejaVuSans.ttf", 20);

    auto canvas = std::make_shared<Canvas>();
    Application::canvas = canvas;

    auto snap_area = canvas->add_child<WindowSnapArea>(0_px, 400_px, Placement::BOTTOM_RIGHT);
    snap_area->mode = WindowSnapArea::Mode::STACKING;

    auto scene_context_menu = canvas->add_child<ContextMenu>();

    canvas->add_child<FPSCounter>(0_px, Placement::TOP_RIGHT);

    auto menu = canvas->add_child<Widget>(0_px, 30_p, Placement::CENTER);
    menu->set_layout<DirectionalLayout>(DirectionalLayout::Direction::RIGHT, 10);
    menu->name = "menu";

    auto closeButton = menu->add_child<Button>(0_px, Placement::BOTTOM_LEFT, "Close");
    closeButton->callback_click = [](InputClickEvent*) {
        glfwSetWindowShouldClose(Application::get_window(), true);
    };

    auto testButton = menu->add_child<Button>(0_px, Placement::BOTTOM_LEFT, "Fenster anzeigen", UIVector(200_px, 50_px));

    auto area = menu->add_child<Textarea>(0_px, 100_px, Placement::BOTTOM_LEFT);
    area->weight = 2;
    area->append("Hallo Welt\nHallo Welt\naaaaaaaa\naaaaaaa\naaaaaa\naaaaaa");

    auto treeWindow = canvas->add_child<Window>(0_px, UIVector(200_px, 300_px));
    treeWindow->set_layout<MaxLayout>();
    treeWindow->title("Scene");

    auto tree = treeWindow->add_child<TreeView>(0_px, 100_p, Placement::TOP_LEFT);
    tree->callback_select = [&](TreeItem& item) {
        if (item.data.type() == typeid(GameObject*))
            Application::selected_object = std::any_cast<GameObject*>(item.data);
    };
    tree->context_menu = scene_context_menu;

    auto& scene_new_menu = scene_context_menu->root_item.add_child("New");
    scene_new_menu.add_child("Empty GameObject", [&]() {
        std::shared_ptr<Scene> scene_ptr;
        if (Application::selected_object && (scene_ptr = Application::scene.lock())) {
            Application::selected_object->add_child();
            tree->sync_scene_tree(scene_ptr.get());
        }
    });
    scene_new_menu.add_child("Plane", [&]() {
        std::shared_ptr<Scene> scene_ptr;
        if (Application::selected_object && (scene_ptr = Application::scene.lock())) {
            auto new_object = Application::selected_object->add_child("Plane");
            new_object->add_component<ModelComponent>("primitive::plane");
            tree->sync_scene_tree(scene_ptr.get());
        }
    });

    scene_context_menu->root_item.add_child("Remove", [&]() {
        if (!Application::selected_object || !Application::selected_object->parent)
            return;
        std::shared_ptr<Scene> scene_ptr;
        if (Application::selected_object && (scene_ptr = Application::scene.lock())) {
            Application::selected_object->remove();
            Application::selected_object = nullptr;
            tree->selected_item(nullptr);
            tree->sync_scene_tree(scene_ptr.get());
        }
    });

    auto testWindow = canvas->add_child<Window>(0_px, 500_px);
    testWindow->set_layout<DirectionalLayout>(DirectionalLayout::Direction::DOWN, 10, true);
    testWindow->hidden = true;
    testWindow->title("Test");

    testWindow->callback_close = [&testWindow]() {
        testWindow->hidden = !testWindow->hidden;
    };

    auto area2 = testWindow->add_child<Textarea>(0_px, UIVector(100_p, 50_px), Placement::TOP_LEFT);
    area2->append("Dies ist ein Fenster");

    auto testCheckBox = testWindow->add_child<CheckBox>(UIVector(0_px, -50_px), Placement::TOP_LEFT, "Textures");

    auto inputR = testWindow->add_child<NumberInput>(UIVector(0_px, -80), UIVector(100_p, 25_px), Placement::TOP_LEFT, 0);
    auto inputG = testWindow->add_child<NumberInput>(UIVector(0_px, -110), UIVector(100_p, 25_px), Placement::TOP_LEFT, 0);
    auto inputB = testWindow->add_child<NumberInput>(UIVector(0_px, -140), UIVector(100_p, 25_px), Placement::TOP_LEFT, 0);
    inputR->min_value = 0;
    inputG->min_value = 0;
    inputB->min_value = 0;
    inputR->max_value = 1;
    inputG->max_value = 1;
    inputB->max_value = 1;
    inputR->value(1);
    inputG->value(1);
    inputB->value(1);

    auto inputX = testWindow->add_child<NumberInput>(UIVector(0_px, -170), UIVector(100_p, 25_px), Placement::TOP_LEFT, 0);
    auto inputY = testWindow->add_child<NumberInput>(UIVector(0_px, -200), UIVector(100_p, 25_px), Placement::TOP_LEFT, 0);
    auto inputZ = testWindow->add_child<NumberInput>(UIVector(0_px, -230), UIVector(100_p, 25_px), Placement::TOP_LEFT, 0);

    testButton->callback_click = [&testWindow](InputClickEvent*) {
        testWindow->hidden = !testWindow->hidden;
    };

    // GameObjects
    auto scene = std::make_shared<Scene>("Scene");
    Application::scene = scene;

    std::shared_ptr<GameObject> flashlight;

    {
        auto player = scene->add_child("Player", glm::vec3(0, 0, 3));
        scene->main_camera = player->add_component<Camera>(800, 600, true);
        player->add_component<FPPlayerController>();

        flashlight = player->add_child("Flashlight", glm::vec3(0), glm::vec3(0));
        flashlight->add_component<Spotlight>(glm::vec3(0), glm::vec3(1), glm::radians(30.0f), glm::radians(40.0f), 0.08f, 0.02f, false);
        flashlight->hidden = true;
    }

    auto redTransparentMaterial = std::make_shared<Material>();
    redTransparentMaterial->diffuse_color = glm::vec4(1.0f, 0.0f, 1.0f, 0.5f);
    auto blueTransparentMaterial = std::make_shared<Material>();
    blueTransparentMaterial->diffuse_color = glm::vec4(0.0f, 1.0f, 1.0f, 0.5f);

    auto obj = scene->add_child("obj", glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f));
    obj->add_component<ModelComponent>("./ressources/testObjects/cube.obj", redTransparentMaterial);
    obj->add_component<Collider>(GenerationMode::COPY);

    auto obj2 = scene->add_child("obj2", glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.0f), glm::vec3(10.0f, 1.0f, 10.0f));
    obj2->add_component<ModelComponent>("./ressources/testObjects/cube.obj", nullptr);

    auto plane = scene->add_child("plane", glm::vec3(2.0f, -4.0f, 2.0f), glm::vec3(0.0f), glm::vec3(10.0f, 1.0f, 10.0f));
    plane->add_component<ModelComponent>("primitive::plane", nullptr);

    auto obj3 = scene->add_child("obj3", glm::vec3(-3.0f, 5.0f, -1.0f), glm::vec3(0.0f));
    obj3->add_component<ModelComponent>("./ressources/testObjects/cube.obj", blueTransparentMaterial);
    obj3->add_component<Collider>(GenerationMode::COPY);

    // Spheres
    auto sphere1 = scene->add_child("Sphere1", glm::vec3(-3.0f, 1.0f, -1.0f), glm::vec3(0), glm::vec3(0.5));
    sphere1->add_component<ModelComponent>("./ressources/testObjects/sphere.obj", nullptr);
    sphere1->add_component<Collider>(GenerationMode::COPY);
    sphere1->add_component<TestComponent>();
    sphere1->add_component<MoveUpDown>(0.4, 1, 5);

    // Light
    auto dirLight = scene->add_child("DirLight", glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(glm::radians(-45.0f), glm::radians(-45.0f), glm::radians(45.0f)));
    dirLight->add_component<DirectionalLight>(glm::vec3(0.2f), glm::vec3(0.7f));
    auto pLight = scene->add_child("Point Light", glm::vec3(2.0f, 1.5f, 4.0f));
    pLight->add_component<PointLight>(glm::vec3(0.2f), glm::vec3(1.0f), 0.09f, 0.032f);
    pLight->add_component<MoveUpDown>(0.1, 1, 3);
    auto sLight = scene->add_child("Spotlight", glm::vec3(-6.0f, 3.0f, -2.0f), glm::vec3(glm::radians(-90.0f), 0, 0));
    sLight->add_component<Spotlight>(glm::vec3(0), glm::vec3(1.0f), glm::radians(40.0f), glm::radians(50.0f), 0.09f, 0.032f);

    Application::event_bus->subscribe<InputKeyEvent>([&flashlight](InputKeyEvent*) {
        flashlight->hidden = !flashlight->hidden;
    },
        GLFW_KEY_F);

    Application::event_bus->subscribe<InputKeyEvent>([&](InputKeyEvent*) {
        pLight->hidden = !pLight->hidden;
    },
        GLFW_KEY_L);

    Application::event_bus->subscribe<InputKeyEvent>([&](InputKeyEvent*) {
        auto random = [](float min, float max) {
            float zeroToOne = ((float)std::rand() / (float)RAND_MAX);
            return zeroToOne * (std::abs(min) + std::abs(max)) + min;
        };
        float x = random(-30, 30);
        float y = random(-30, 30);
        float z = random(-30, 30);
        auto newMaterial = std::make_shared<Material>();
        newMaterial->diffuse_color = glm::vec4(random(0, 1), random(0, 1), random(0, 1), 1.0f);
        auto newCube = scene->add_child("New Cube", glm::vec3(x, y, z));
        newCube->add_component<ModelComponent>("./ressources/testObjects/cube.obj", newMaterial);
        tree->sync_scene_tree(scene.get());
        Logger::debug("Created cube at x: ", x, " y: ", y, " z: ", z);
    },
        GLFW_KEY_N);

    Application::event_bus->subscribe<InputKeyEvent>([](InputKeyEvent*) {
       Application::option_toggle(Option::VSYNC);
    },
        GLFW_KEY_V);

    Application::event_bus->subscribe<InputKeyEvent>([](InputKeyEvent*) {
       Application::option_toggle(Option::SHOW_COLLIDERS);
    },
        GLFW_KEY_P);

    scene->start();

    tree->sync_scene_tree(scene.get());

    //Mainloop
    Application::mainloop();

    scene->cleanup();
    Application::cleanup();

    return 0;
}
