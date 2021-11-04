#include "Birdy3d.hpp"
#include <filesystem>
#include <fstream>
#ifdef BIRDY3D_PLATFORM_LINUX
    #include <cstdlib>
    #include <execinfo.h>
    #include <signal.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
#endif

using namespace Birdy3d;

class TestComponent : public Component {
public:
    void start() override {
        Application::event_bus->subscribe(this, &TestComponent::on_collision);
    }

    void cleanup() override {
        Application::event_bus->unsubscribe(this, &TestComponent::on_collision);
    }

    void on_collision(const CollisionEvent& event) {
        switch (event.type) {
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

    BIRDY3D_REGISTER_DERIVED_TYPE_DEC(Component, TestComponent);
};

BIRDY3D_REGISTER_DERIVED_TYPE_DEF(Component, TestComponent);

class MoveUpDown : public Component {
public:
    MoveUpDown(float speed = 0, float limit_down = 0, float limit_up = 0)
        : m_speed(speed)
        , m_limit_down(limit_down)
        , m_limit_up(limit_up) { }

    void update() override {
        if (m_up) {
            entity->transform.position.y += m_speed * Application::delta_time;
            if (entity->transform.position.y > m_limit_up)
                m_up = false;
        } else {
            entity->transform.position.y -= m_speed * Application::delta_time;
            if (entity->transform.position.y < m_limit_down)
                m_up = true;
        }
    }

    void serialize(serializer::Adapter& adapter) override {
        adapter("speed", m_speed);
        adapter("limit_down", m_limit_down);
        adapter("limit_up", m_limit_up);
    }

private:
    bool m_up = true;
    float m_speed;
    float m_limit_down;
    float m_limit_up;

    BIRDY3D_REGISTER_DERIVED_TYPE_DEC(Component, MoveUpDown);
};

BIRDY3D_REGISTER_DERIVED_TYPE_DEF(Component, MoveUpDown);

#ifdef BIRDY3D_PLATFORM_LINUX
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
#endif

int main() {
#ifdef BIRDY3D_PLATFORM_LINUX
    signal(SIGSEGV, handler);
#endif

    if (!Application::init("Birdy3d", 1280, 720, "gruvbox.json")) {
        return -1;
    }
    Input::init();

    std::shared_ptr<Birdy3d::NumberInput> input_position_x;
    std::shared_ptr<Birdy3d::NumberInput> input_position_y;
    std::shared_ptr<Birdy3d::NumberInput> input_position_z;
    std::shared_ptr<Birdy3d::NumberInput> input_scale_x;
    std::shared_ptr<Birdy3d::NumberInput> input_scale_y;
    std::shared_ptr<Birdy3d::NumberInput> input_scale_z;
    std::shared_ptr<Birdy3d::NumberInput> input_orientation_x;
    std::shared_ptr<Birdy3d::NumberInput> input_orientation_y;
    std::shared_ptr<Birdy3d::NumberInput> input_orientation_z;

    // UI
    auto canvas = std::make_shared<Canvas>();
    Application::canvas = canvas;

    auto menu_bar = canvas->add_child<MenuBar>(0_px, UIVector(100_p, Application::theme->line_height()), Placement::TOP_LEFT);
    auto& bar_item1 = menu_bar->add_item("Item 1");
    bar_item1.add_child("Hallo");
    auto& bar_item2 = menu_bar->add_item("Item 2");
    bar_item2.add_child("Welt");

    auto snap_area = canvas->add_child<WindowSnapArea>(0_px, UIVector(600_px, 400_px), Placement::BOTTOM_RIGHT);
    snap_area->mode = WindowSnapArea::Mode::HORIZONTAL;

    auto scene_context_menu = canvas->add_child<ContextMenu>();

    canvas->add_child<FPSCounter>(0_px, Placement::TOP_RIGHT);

    auto menu = canvas->add_child<Widget>(0_px, 30_p, Placement::CENTER);
    menu->set_layout<DirectionalLayout>(DirectionalLayout::Direction::RIGHT, 10);
    menu->name = "menu";

    auto close_button = menu->add_child<Button>(0_px, Placement::BOTTOM_LEFT, "Close");
    close_button->callback_click = [](const InputClickEvent&) {
        glfwSetWindowShouldClose(Application::get_window(), true);
    };

    auto test_button = menu->add_child<Button>(0_px, Placement::BOTTOM_LEFT, "Fenster anzeigen", UIVector(200_px, 50_px));

    auto area = menu->add_child<Textarea>(0_px, 100_px, Placement::BOTTOM_LEFT);
    area->weight = 2;
    area->append("Hallo Welt\nHallo Welt\naaaaaaaa\naaaaaaa\naaaaaa\naaaaaa");

    auto tree_window = canvas->add_child<Window>(0_px, UIVector(200_px, 300_px));
    tree_window->set_layout<MaxLayout>();
    tree_window->title("Scene");

    auto tree_scroll_view = tree_window->add_child<ScrollView>(0_px, 100_p);
    tree_scroll_view->set_layout<MaxLayout>();

    auto tree = tree_scroll_view->add_child<TreeView>(0_px, 100_p, Placement::TOP_LEFT);
    tree->callback_select = [&](TreeItem& item) {
        if (item.data.type() == typeid(Entity*)) {
            Application::selected_entity = std::any_cast<Entity*>(item.data);
            input_position_x->value(Application::selected_entity->transform.position.x);
            input_position_y->value(Application::selected_entity->transform.position.y);
            input_position_z->value(Application::selected_entity->transform.position.z);
            input_scale_x->value(Application::selected_entity->transform.scale.x);
            input_scale_y->value(Application::selected_entity->transform.scale.y);
            input_scale_z->value(Application::selected_entity->transform.scale.z);
            input_orientation_x->value(Application::selected_entity->transform.orientation.x);
            input_orientation_y->value(Application::selected_entity->transform.orientation.y);
            input_orientation_z->value(Application::selected_entity->transform.orientation.z);
        }
    };
    tree->context_menu = scene_context_menu;

    auto& scene_new_menu = scene_context_menu->root_item.add_child("New");
    scene_new_menu.add_child("Empty Entity", [&]() {
        std::shared_ptr<Scene> scene_ptr;
        if (Application::selected_entity && (scene_ptr = Application::scene.lock())) {
            Application::selected_entity->add_child();
            tree->sync_scene_tree(scene_ptr.get());
        }
    });
    scene_new_menu.add_child("Plane", [&]() {
        std::shared_ptr<Scene> scene_ptr;
        if (Application::selected_entity && (scene_ptr = Application::scene.lock())) {
            auto new_entity = Application::selected_entity->add_child("Plane");
            new_entity->add_component<ModelComponent>("primitive::plane");
            tree->sync_scene_tree(scene_ptr.get());
        }
    });
    scene_new_menu.add_child("Cube", [&]() {
        std::shared_ptr<Scene> scene_ptr;
        if (Application::selected_entity && (scene_ptr = Application::scene.lock())) {
            auto new_entity = Application::selected_entity->add_child("Cube");
            new_entity->add_component<ModelComponent>("primitive::cube");
            tree->sync_scene_tree(scene_ptr.get());
        }
    });

    scene_context_menu->root_item.add_child("Remove", [&]() {
        if (!Application::selected_entity || !Application::selected_entity->parent)
            return;
        std::shared_ptr<Scene> scene_ptr;
        if (Application::selected_entity && (scene_ptr = Application::scene.lock())) {
            Application::selected_entity->remove();
            Application::selected_entity = nullptr;
            tree->selected_item(nullptr);
            tree->sync_scene_tree(scene_ptr.get());
        }
    });

    auto inspector_window = canvas->add_child<Window>(0_px, 500_px);
    inspector_window->set_layout<MaxLayout>();
    inspector_window->hidden = true;
    inspector_window->title("Inspector");

    inspector_window->callback_close = [&inspector_window]() {
        inspector_window->hidden = !inspector_window->hidden;
    };

    auto inspector_scroll_view = inspector_window->add_child<ScrollView>(0_px, 100_p, Placement::BOTTOM_LEFT);
    inspector_scroll_view->set_layout<DirectionalLayout>(DirectionalLayout::Direction::DOWN, 10, true);

    auto transform_box = inspector_scroll_view->add_child<CollapsibleBox>(UIVector(0_px, 5_px));
    transform_box->title("Transform");
    transform_box->set_layout<DirectionalLayout>(DirectionalLayout::Direction::DOWN, 10, true);

    input_position_x = transform_box->add_child<NumberInput>(0_px, UIVector(100_p, 25_px), Placement::BOTTOM_LEFT, 0);
    input_position_y = transform_box->add_child<NumberInput>(0_px, UIVector(100_p, 25_px), Placement::BOTTOM_LEFT, 0);
    input_position_z = transform_box->add_child<NumberInput>(0_px, UIVector(100_p, 25_px), Placement::BOTTOM_LEFT, 0);

    input_position_x->callback_change = [&] {
        if (Application::selected_entity)
            Application::selected_entity->transform.position.x = input_position_x->value();
    };

    input_position_y->callback_change = [&] {
        if (Application::selected_entity)
            Application::selected_entity->transform.position.y = input_position_y->value();
    };

    input_position_z->callback_change = [&] {
        if (Application::selected_entity)
            Application::selected_entity->transform.position.z = input_position_z->value();
    };

    input_scale_x = transform_box->add_child<NumberInput>(0_px, UIVector(100_p, 25_px), Placement::BOTTOM_LEFT, 0);
    input_scale_y = transform_box->add_child<NumberInput>(0_px, UIVector(100_p, 25_px), Placement::BOTTOM_LEFT, 0);
    input_scale_z = transform_box->add_child<NumberInput>(0_px, UIVector(100_p, 25_px), Placement::BOTTOM_LEFT, 0);

    input_scale_x->min_value = 0;
    input_scale_y->min_value = 0;
    input_scale_z->min_value = 0;

    input_scale_x->callback_change = [&] {
        if (Application::selected_entity)
            Application::selected_entity->transform.scale.x = input_scale_x->value();
    };

    input_scale_y->callback_change = [&] {
        if (Application::selected_entity)
            Application::selected_entity->transform.scale.y = input_scale_y->value();
    };

    input_scale_z->callback_change = [&] {
        if (Application::selected_entity)
            Application::selected_entity->transform.scale.z = input_scale_z->value();
    };

    input_orientation_x = transform_box->add_child<NumberInput>(0_px, UIVector(100_p, 25_px), Placement::BOTTOM_LEFT, 0);
    input_orientation_y = transform_box->add_child<NumberInput>(0_px, UIVector(100_p, 25_px), Placement::BOTTOM_LEFT, 0);
    input_orientation_z = transform_box->add_child<NumberInput>(0_px, UIVector(100_p, 25_px), Placement::BOTTOM_LEFT, 0);

    input_orientation_x->callback_change = [&] {
        if (Application::selected_entity)
            Application::selected_entity->transform.orientation.x = input_orientation_x->value();
    };

    input_orientation_y->callback_change = [&] {
        if (Application::selected_entity)
            Application::selected_entity->transform.orientation.y = input_orientation_y->value();
    };

    input_orientation_z->callback_change = [&] {
        if (Application::selected_entity)
            Application::selected_entity->transform.orientation.z = input_orientation_z->value();
    };

    Application::event_bus->subscribe<TransformChangedEvent>([&](const TransformChangedEvent& event) {
        if (event.entity != Birdy3d::Application::selected_entity)
            return;
        input_position_x->value(event.entity->transform.position.x);
        input_position_y->value(event.entity->transform.position.y);
        input_position_z->value(event.entity->transform.position.z);
        input_scale_x->value(event.entity->transform.scale.x);
        input_scale_y->value(event.entity->transform.scale.y);
        input_scale_z->value(event.entity->transform.scale.z);
        input_orientation_x->value(event.entity->transform.orientation.x);
        input_orientation_y->value(event.entity->transform.orientation.y);
        input_orientation_z->value(event.entity->transform.orientation.z);
    });

    auto test_checkbox = inspector_scroll_view->add_child<CheckBox>(UIVector(0_px, -50_px), Placement::TOP_LEFT, "Textures");

    test_button->callback_click = [&inspector_window](const InputClickEvent&) {
        inspector_window->hidden = !inspector_window->hidden;
    };

    //auto test_window = canvas->add_child<Window>(UIVector(0_px, 50_px), 200_px);
    //test_window->title("Test");

    // Entities
    std::shared_ptr<Scene> scene;
    if (std::filesystem::exists("scene.json")) {
        serializer::JsonParser parser(RessourceManager::read_file("scene.json"));
        serializer::Serializer::deserialize(RessourceManager::read_file("scene.json"), "scene", scene);
        Application::scene = scene;
    } else {
        scene = std::make_shared<Scene>("Scene");
        Application::scene = scene;

        auto player = scene->add_child("Player", glm::vec3(0, 0, 3));
        auto viewport = Application::get_viewport_size();
        scene->main_camera = player->add_component<Camera>(viewport.x, viewport.y, true);
        auto player_controller = player->add_component<FPPlayerController>();

        auto flashlight = player->add_child("Flashlight", glm::vec3(0), glm::vec3(0));
        flashlight->add_component<Spotlight>(glm::vec3(0), glm::vec3(1), glm::radians(30.0f), glm::radians(40.0f), 0.08f, 0.02f, false);
        flashlight->hidden = true;

        player_controller->flashlight = flashlight;

        auto white_material = std::make_shared<Material>();
        white_material->specular_value = 1.0f;
        auto red_transparent_material = std::make_shared<Material>();
        red_transparent_material->diffuse_color = glm::vec4(1.0f, 0.0f, 1.0f, 0.5f);
        auto blue_transparent_material = std::make_shared<Material>();
        blue_transparent_material->diffuse_color = glm::vec4(0.0f, 1.0f, 1.0f, 0.5f);

        auto obj = scene->add_child("obj", glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f));
        obj->add_component<ModelComponent>("primitive::cube", red_transparent_material);
        obj->add_component<Collider>(GenerationMode::COPY);

        auto obj2 = scene->add_child("obj2", glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.0f), glm::vec3(10.0f, 1.0f, 10.0f));
        obj2->add_component<ModelComponent>("primitive::cube", white_material);

        auto plane = scene->add_child("plane", glm::vec3(2.0f, -4.0f, 2.0f), glm::vec3(0.0f), glm::vec3(10.0f, 1.0f, 10.0f));
        plane->add_component<ModelComponent>("primitive::plane", white_material);

        auto obj3 = scene->add_child("obj3", glm::vec3(-3.0f, 5.0f, -1.0f), glm::vec3(0.0f));
        obj3->add_component<ModelComponent>("primitive::cube", blue_transparent_material);
        obj3->add_component<Collider>(GenerationMode::COPY);

        // Spheres
        auto sphere1 = scene->add_child("Sphere1", glm::vec3(-3.0f, 1.0f, -1.0f), glm::vec3(0), glm::vec3(0.5));
        sphere1->add_component<ModelComponent>("primitive::uv_sphere:20", nullptr);
        sphere1->add_component<Collider>(GenerationMode::COPY);
        sphere1->add_component<TestComponent>();
        sphere1->add_component<MoveUpDown>(0.4, 1, 5);

        // Light
        auto dir_light = scene->add_child("DirLight", glm::vec3(0.2f, 3.0f, 0.0f), glm::vec3(glm::radians(-45.0f), glm::radians(-45.0f), glm::radians(45.0f)));
        dir_light->add_component<DirectionalLight>(glm::vec3(0.2f), glm::vec3(0.7f));
        auto point_light = scene->add_child("Point Light", glm::vec3(2.0f, 1.5f, 4.0f));
        point_light->add_component<PointLight>(glm::vec3(0.2f), glm::vec3(1.0f), 0.09f, 0.032f);
        point_light->add_component<MoveUpDown>(0.1, 1, 3);
        auto spot_light = scene->add_child("Spotlight", glm::vec3(-6.0f, 3.0f, -2.0f), glm::vec3(glm::radians(-90.0f), 0, 0));
        spot_light->add_component<Spotlight>(glm::vec3(0), glm::vec3(1.0f), glm::radians(40.0f), glm::radians(50.0f), 0.09f, 0.032f);

        Application::event_bus->subscribe<InputKeyEvent>([point_light](const InputKeyEvent&) {
            point_light->hidden = !point_light->hidden;
        },
            GLFW_KEY_L);
    }

    Application::event_bus->subscribe<InputKeyEvent>([&](const InputKeyEvent&) {
        auto random = [](float min, float max) {
            float zero_to_one = ((float)std::rand() / (float)RAND_MAX);
            return zero_to_one * (std::abs(min) + std::abs(max)) + min;
        };
        float x = random(-30, 30);
        float y = random(-30, 30);
        float z = random(-30, 30);
        auto new_material = std::make_shared<Material>();
        new_material->diffuse_color = glm::vec4(random(0, 1), random(0, 1), random(0, 1), 1.0f);
        auto new_cube = scene->add_child("New Cube", glm::vec3(x, y, z));
        new_cube->add_component<ModelComponent>("primitive::cube", new_material);
        tree->sync_scene_tree(scene.get());
        Logger::debug("Created cube at x: ", x, " y: ", y, " z: ", z);
    },
        GLFW_KEY_N);

    Application::event_bus->subscribe<InputKeyEvent>([](const InputKeyEvent&) {
        Application::option_toggle(Option::VSYNC);
    },
        GLFW_KEY_V);

    Application::event_bus->subscribe<InputKeyEvent>([](const InputKeyEvent&) {
        Application::option_toggle(Option::SHOW_COLLIDERS);
    },
        GLFW_KEY_P);

    scene->start();

    tree->sync_scene_tree(scene.get());

    //Mainloop
    Application::mainloop();

    scene->cleanup();
    Application::cleanup();

    std::fstream filestream;
    filestream.open("scene.json", std::fstream::out);
    serializer::Serializer::serialize(serializer::GeneratorType::JSON_PRETTY, "scene", scene, filestream);
    filestream.close();

    return 0;
}
