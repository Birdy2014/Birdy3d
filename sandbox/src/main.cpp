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

class TestComponent : public ecs::Component {
public:
    void start() override {
        core::Application::event_bus->subscribe(this, &TestComponent::on_collision);
    }

    void cleanup() override {
        core::Application::event_bus->unsubscribe(this, &TestComponent::on_collision);
    }

    void on_collision(const events::CollisionEvent& event) {
        switch (event.type) {
        case events::CollisionEvent::ENTER:
            core::Logger::debug("ENTER");
            break;
        case events::CollisionEvent::COLLIDING:
            break;
        case events::CollisionEvent::EXIT:
            core::Logger::debug("EXIT");
            break;
        }
    }

    BIRDY3D_REGISTER_DERIVED_TYPE_DEC(ecs::Component, TestComponent);
};

BIRDY3D_REGISTER_DERIVED_TYPE_DEF(ecs::Component, TestComponent);

class MoveUpDown : public ecs::Component {
public:
    MoveUpDown(float speed = 0, float limit_down = 0, float limit_up = 0)
        : m_speed(speed)
        , m_limit_down(limit_down)
        , m_limit_up(limit_up) { }

    void update() override {
        if (m_up) {
            entity->transform.position.y += m_speed * core::Application::delta_time;
            if (entity->transform.position.y > m_limit_up)
                m_up = false;
        } else {
            entity->transform.position.y -= m_speed * core::Application::delta_time;
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

    BIRDY3D_REGISTER_DERIVED_TYPE_DEC(ecs::Component, MoveUpDown);
};

BIRDY3D_REGISTER_DERIVED_TYPE_DEF(ecs::Component, MoveUpDown);

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

    if (!core::Application::init("Birdy3d", 1280, 720, "gruvbox-dark.json")) {
        return -1;
    }
    core::Input::init();

    std::shared_ptr<ui::NumberInput> input_position_x;
    std::shared_ptr<ui::NumberInput> input_position_y;
    std::shared_ptr<ui::NumberInput> input_position_z;
    std::shared_ptr<ui::NumberInput> input_scale_x;
    std::shared_ptr<ui::NumberInput> input_scale_y;
    std::shared_ptr<ui::NumberInput> input_scale_z;
    std::shared_ptr<ui::NumberInput> input_orientation_x;
    std::shared_ptr<ui::NumberInput> input_orientation_y;
    std::shared_ptr<ui::NumberInput> input_orientation_z;
    std::shared_ptr<ui::Container> inspector_component_container;

    // UI
    auto canvas = std::make_shared<ui::Canvas>();
    core::Application::canvas = canvas;

    auto menu_bar = canvas->add_child<ui::MenuBar>({ .size = ui::UIVector(100_p, core::Application::theme().line_height()), .placement = ui::Placement::TOP_LEFT });
    auto& bar_item1 = menu_bar->add_item("Item 1");
    bar_item1.add_child("Hallo");
    auto& bar_item2 = menu_bar->add_item("Item 2");
    bar_item2.add_child("Welt");

    ui::Console::attach(*canvas);

    auto snap_area = canvas->add_child<ui::WindowSnapArea>({ .size = ui::UIVector(600_px, 400_px), .placement = ui::Placement::BOTTOM_RIGHT }, ui::WindowSnapArea::Mode::HORIZONTAL);

    auto scene_context_menu = canvas->add_child<ui::ContextMenu>({});

    canvas->add_child<ui::FPSCounter>({ .placement = ui::Placement::TOP_RIGHT });

    auto menu = canvas->add_child<ui::Container>({ .size = 30_p, .placement = ui::Placement::CENTER, .name = "menu" });
    menu->set_layout<ui::DirectionalLayout>(ui::DirectionalLayout::Direction::RIGHT, 10);

    auto close_button = menu->add_child<ui::Button>({ .placement = ui::Placement::BOTTOM_LEFT }, "Close");
    close_button->callback_click = [](const events::InputClickEvent&) {
        glfwSetWindowShouldClose(core::Application::get_window(), true);
    };

    auto test_button = menu->add_child<ui::Button>({ .size = ui::UIVector(200_px, 50_px), .placement = ui::Placement::BOTTOM_LEFT }, "Fenster anzeigen");

    auto area = menu->add_child<ui::Textarea>({ .weight = 2 });
    area->append("Hallo Welt\nHallo Welt\naaaaaaaa\naaaaaaa\naaaaaa\naaaaaa");

    auto tree_window = canvas->add_child<ui::Window>({ .size = ui::UIVector(200_px, 300_px) });
    tree_window->set_layout<ui::MaxLayout>();
    tree_window->title("Scene");

    auto tree_scroll_view = tree_window->add_child<ui::ScrollContainer>({ .pos = 0_px, .size = 100_p });
    tree_scroll_view->set_layout<ui::MaxLayout>();

    auto tree = tree_scroll_view->add_child<ui::TreeView>({ .size = 100_p, .placement = ui::Placement::TOP_LEFT });
    tree->callback_select = [&](ui::TreeItem& item) {
        if (item.data.type() != typeid(ecs::Entity*))
            return;

        core::Application::selected_entity = std::any_cast<ecs::Entity*>(item.data);
        input_position_x->value(core::Application::selected_entity->transform.position.x);
        input_position_y->value(core::Application::selected_entity->transform.position.y);
        input_position_z->value(core::Application::selected_entity->transform.position.z);
        input_scale_x->value(core::Application::selected_entity->transform.scale.x);
        input_scale_y->value(core::Application::selected_entity->transform.scale.y);
        input_scale_z->value(core::Application::selected_entity->transform.scale.z);
        input_orientation_x->value(core::Application::selected_entity->transform.orientation.x);
        input_orientation_y->value(core::Application::selected_entity->transform.orientation.y);
        input_orientation_z->value(core::Application::selected_entity->transform.orientation.z);

        // Components
        inspector_component_container->clear_children();
        for (const auto& component : core::Application::selected_entity->components()) {
            const auto& c = serializer::Reflector::get_class(component.get());
            auto box = inspector_component_container->add_child<ui::CollapsibleContainer>({ .pos = ui::UIVector(0_px, 5_px), .size = ui::UIVector(100_p, 0_px) }, c.name);
            box->set_layout<ui::GridLayout>(5);
            int current_row = 0;
            for (const auto& member : c.m_members) {
                auto label = box->add_child<ui::Label>({ .placement = ui::Placement::BOTTOM_LEFT, .column = 0, .row = current_row }, member.name);
                auto widget_options = ui::Widget::Options { .size = ui::UIVector(100_p, 20_px), .column = 1, .row = current_row };
                if (member.type == typeid(std::string)) {
                    std::weak_ptr<ui::TextField> text_field = box->add_child<ui::TextField>(widget_options);
                    text_field.lock()->text(*(std::string*)member.value);
                    text_field.lock()->add_callback("change", [text_field, member]() {
                        *(std::string*)member.value = text_field.lock()->text();
                    });
                } else if (member.type == typeid(bool)) {
                    std::weak_ptr<ui::CheckBox> checkbox = box->add_child<ui::CheckBox>(widget_options, member.name);
                    checkbox.lock()->checked = *(bool*)member.value;
                    checkbox.lock()->add_callback("change", [checkbox, member]() {
                        *(bool*)member.value = checkbox.lock()->checked;
                    });
                } else if (member.type == typeid(int)) {
                    std::weak_ptr<ui::NumberInput> number_input = box->add_child<ui::NumberInput>(widget_options);
                    number_input.lock()->value(*(int*)member.value);
                    number_input.lock()->add_callback("change", [number_input, member]() {
                        *(int*)member.value = number_input.lock()->value();
                    });
                } else if (member.type == typeid(float)) {
                    std::weak_ptr<ui::NumberInput> number_input = box->add_child<ui::NumberInput>(widget_options);
                    number_input.lock()->value(*(float*)member.value);
                    number_input.lock()->add_callback("change", [number_input, member]() {
                        *(float*)member.value = number_input.lock()->value();
                    });
                } else if (member.type == typeid(core::ResourceHandle<render::Shader>)) {
                    std::weak_ptr<ui::TextField> text_field = box->add_child<ui::TextField>(widget_options);
                    text_field.lock()->text(*(core::ResourceHandle<render::Shader>*)member.value);
                    text_field.lock()->add_callback("change", [text_field, member]() {
                        *(core::ResourceHandle<render::Shader>*)member.value = text_field.lock()->text();
                    });
                } else if (member.type == typeid(core::ResourceHandle<ui::Theme>)) {
                    std::weak_ptr<ui::TextField> text_field = box->add_child<ui::TextField>(widget_options);
                    text_field.lock()->text(*(core::ResourceHandle<ui::Theme>*)member.value);
                    text_field.lock()->add_callback("change", [text_field, member]() {
                        *(core::ResourceHandle<ui::Theme>*)member.value = text_field.lock()->text();
                    });
                } else if (member.type == typeid(core::ResourceHandle<render::Model>)) {
                    std::weak_ptr<ui::TextField> text_field = box->add_child<ui::TextField>(widget_options);
                    text_field.lock()->text(*(core::ResourceHandle<render::Model>*)member.value);
                    text_field.lock()->add_callback("change", [text_field, member]() {
                        *(core::ResourceHandle<render::Model>*)member.value = text_field.lock()->text();
                    });
                } else if (member.type == typeid(core::ResourceHandle<render::Texture>)) {
                    std::weak_ptr<ui::TextField> text_field = box->add_child<ui::TextField>(widget_options);
                    text_field.lock()->text(*(core::ResourceHandle<render::Texture>*)member.value);
                    text_field.lock()->add_callback("change", [text_field, member]() {
                        *(core::ResourceHandle<render::Texture>*)member.value = text_field.lock()->text();
                    });
                } else {
                    std::cout << member.name << '\n';
                }
                ++current_row;
            }
        }
    };
    tree->context_menu = scene_context_menu;

    auto& scene_new_menu = scene_context_menu->root_item.add_child("New");
    scene_new_menu.add_child("Empty Entity", [&]() {
        std::shared_ptr<ecs::Scene> scene_ptr;
        if (core::Application::selected_entity && (scene_ptr = core::Application::scene.lock())) {
            core::Application::selected_entity->add_child();
            tree->sync_scene_tree(scene_ptr.get());
        }
    });
    scene_new_menu.add_child("Plane", [&]() {
        std::shared_ptr<ecs::Scene> scene_ptr;
        if (core::Application::selected_entity && (scene_ptr = core::Application::scene.lock())) {
            auto new_entity = core::Application::selected_entity->add_child("Plane");
            new_entity->add_component<render::ModelComponent>("primitive::plane");
            tree->sync_scene_tree(scene_ptr.get());
        }
    });
    scene_new_menu.add_child("Cube", [&]() {
        std::shared_ptr<ecs::Scene> scene_ptr;
        if (core::Application::selected_entity && (scene_ptr = core::Application::scene.lock())) {
            auto new_entity = core::Application::selected_entity->add_child("Cube");
            new_entity->add_component<render::ModelComponent>("primitive::cube");
            tree->sync_scene_tree(scene_ptr.get());
        }
    });

    scene_context_menu->root_item.add_child("Copy", [&]() {
        if (!core::Application::selected_entity || !core::Application::selected_entity->parent)
            return;
        if (auto scene_ptr = core::Application::scene.lock()) {
            auto cloned_entity = core::Application::selected_entity->clone();
            core::Application::selected_entity->parent->add_child(cloned_entity);
            core::Application::selected_entity = cloned_entity.get();
            cloned_entity->start();
            tree->selected_item(nullptr);
            tree->sync_scene_tree(scene_ptr.get());
        }
    });

    scene_context_menu->root_item.add_child("Remove", [&]() {
        if (!core::Application::selected_entity || !core::Application::selected_entity->parent)
            return;
        if (auto scene_ptr = core::Application::scene.lock()) {
            core::Application::selected_entity->remove();
            core::Application::selected_entity = nullptr;
            tree->selected_item(nullptr);
            tree->sync_scene_tree(scene_ptr.get());
        }
    });

    auto inspector_window = canvas->add_child<ui::Window>({ .size = 500_px, .hidden = true });
    inspector_window->set_layout<ui::MaxLayout>();
    inspector_window->title("Inspector");

    inspector_window->callback_close = [&inspector_window]() {
        inspector_window->options.hidden = !inspector_window->options.hidden;
    };

    test_button->callback_click = [&inspector_window](const events::InputClickEvent&) {
        inspector_window->options.hidden = !inspector_window->options.hidden;
    };

    auto inspector_scroll_view = inspector_window->add_child<ui::ScrollContainer>({ .size = 100_p, .placement = ui::Placement::BOTTOM_LEFT });
    inspector_scroll_view->set_layout<ui::DirectionalLayout>(ui::DirectionalLayout::Direction::DOWN, 10, true);

    auto transform_box = inspector_scroll_view->add_child<ui::CollapsibleContainer>({ .pos = ui::UIVector(0_px, 5_px), .size = ui::UIVector(100_p, 0_px) }, "Transform");
    transform_box->set_layout<ui::GridLayout>(5);

    auto position_label = transform_box->add_child<ui::Label>({ .placement = ui::Placement::CENTER_LEFT, .column = 0, .row = 0 }, "position");

    auto position_box = transform_box->add_child<ui::Container>({ .size = ui::UIVector(100_p, 0_px), .column = 1, .row = 0 });
    position_box->set_layout<ui::DirectionalLayout>(ui::DirectionalLayout::Direction::RIGHT, 5, false);
    input_position_x = position_box->add_child<ui::NumberInput>({ .size = ui::UIVector(100_p, 25_px), .placement = ui::Placement::BOTTOM_LEFT }, 0);
    input_position_y = position_box->add_child<ui::NumberInput>({ .size = ui::UIVector(100_p, 25_px), .placement = ui::Placement::BOTTOM_LEFT }, 0);
    input_position_z = position_box->add_child<ui::NumberInput>({ .size = ui::UIVector(100_p, 25_px), .placement = ui::Placement::BOTTOM_LEFT }, 0);

    input_position_x->add_callback("change", [&] {
        if (core::Application::selected_entity)
            core::Application::selected_entity->transform.position.x = input_position_x->value();
    });

    input_position_y->add_callback("change", [&] {
        if (core::Application::selected_entity)
            core::Application::selected_entity->transform.position.y = input_position_y->value();
    });

    input_position_z->add_callback("change", [&] {
        if (core::Application::selected_entity)
            core::Application::selected_entity->transform.position.z = input_position_z->value();
    });

    auto scale_label = transform_box->add_child<ui::Label>({ .placement = ui::Placement::CENTER_LEFT, .column = 0, .row = 1 }, "scale");

    auto scale_box = transform_box->add_child<ui::Container>({ .size = ui::UIVector(100_p, 0_px), .column = 1, .row = 1 });
    scale_box->set_layout<ui::DirectionalLayout>(ui::DirectionalLayout::Direction::RIGHT, 5, false);
    input_scale_x = scale_box->add_child<ui::NumberInput>({ .size = ui::UIVector(100_p, 25_px) }, 0);
    input_scale_y = scale_box->add_child<ui::NumberInput>({ .size = ui::UIVector(100_p, 25_px) }, 0);
    input_scale_z = scale_box->add_child<ui::NumberInput>({ .size = ui::UIVector(100_p, 25_px) }, 0);

    input_scale_x->min_value = 0;
    input_scale_y->min_value = 0;
    input_scale_z->min_value = 0;

    input_scale_x->add_callback("change", [&] {
        if (core::Application::selected_entity)
            core::Application::selected_entity->transform.scale.x = input_scale_x->value();
    });

    input_scale_y->add_callback("change", [&] {
        if (core::Application::selected_entity)
            core::Application::selected_entity->transform.scale.y = input_scale_y->value();
    });

    input_scale_z->add_callback("change", [&] {
        if (core::Application::selected_entity)
            core::Application::selected_entity->transform.scale.z = input_scale_z->value();
    });

    auto orientation_label = transform_box->add_child<ui::Label>({ .placement = ui::Placement::CENTER_LEFT, .column = 0, .row = 2 }, "orientation");

    auto orientation_box = transform_box->add_child<ui::Container>({ .size = ui::UIVector(100_p, 0_px), .column = 1, .row = 2 });
    orientation_box->set_layout<ui::DirectionalLayout>(ui::DirectionalLayout::Direction::RIGHT, 5, false);
    input_orientation_x = orientation_box->add_child<ui::NumberInput>({ .size = ui::UIVector(100_p, 25_px), .placement = ui::Placement::BOTTOM_LEFT }, 0);
    input_orientation_y = orientation_box->add_child<ui::NumberInput>({ .size = ui::UIVector(100_p, 25_px), .placement = ui::Placement::BOTTOM_LEFT }, 0);
    input_orientation_z = orientation_box->add_child<ui::NumberInput>({ .size = ui::UIVector(100_p, 25_px), .placement = ui::Placement::BOTTOM_LEFT }, 0);

    input_orientation_x->add_callback("change", [&] {
        if (core::Application::selected_entity)
            core::Application::selected_entity->transform.orientation.x = input_orientation_x->value();
    });

    input_orientation_y->add_callback("change", [&] {
        if (core::Application::selected_entity)
            core::Application::selected_entity->transform.orientation.y = input_orientation_y->value();
    });

    input_orientation_z->add_callback("change", [&] {
        if (core::Application::selected_entity)
            core::Application::selected_entity->transform.orientation.z = input_orientation_z->value();
    });

    core::Application::event_bus->subscribe<events::TransformChangedEvent>([&](const events::TransformChangedEvent& event) {
        if (event.entity != core::Application::selected_entity)
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

    inspector_component_container = inspector_scroll_view->add_child<ui::Container>({ .size = ui::UIVector(100_p, 0_px) });
    inspector_component_container->set_layout<ui::DirectionalLayout>(ui::DirectionalLayout::Direction::DOWN, 10, true);

    // Entities
    std::shared_ptr<ecs::Scene> scene;
    if (std::filesystem::exists("scene.json")) {
        serializer::JsonParser parser(core::ResourceManager::read_file("scene.json"));
        serializer::Serializer::deserialize(core::ResourceManager::read_file("scene.json"), "scene", scene);
        core::Application::scene = scene;
    } else {
        scene = std::make_shared<ecs::Scene>("Scene");
        core::Application::scene = scene;

        auto player = scene->add_child("Player", glm::vec3(0, 0, 3));
        auto viewport = core::Application::get_viewport_size();
        scene->main_camera = player->add_component<render::Camera>(viewport.x, viewport.y, true);
        auto player_controller = player->add_component<utils::FPPlayerController>();

        auto flashlight = player->add_child("Flashlight", glm::vec3(0), glm::vec3(0));
        flashlight->add_component<render::Spotlight>(glm::vec3(0), glm::vec3(1), glm::radians(30.0f), glm::radians(40.0f), 0.08f, 0.02f, false);
        flashlight->hidden = true;

        player_controller->flashlight = flashlight;

        auto white_material = std::make_shared<render::Material>();
        white_material->specular_value = 1.0f;
        auto red_transparent_material = std::make_shared<render::Material>();
        red_transparent_material->diffuse_color = glm::vec4(1.0f, 0.0f, 1.0f, 0.5f);
        auto blue_transparent_material = std::make_shared<render::Material>();
        blue_transparent_material->diffuse_color = glm::vec4(0.0f, 1.0f, 1.0f, 0.5f);

        auto obj = scene->add_child("obj", glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f));
        obj->add_component<physics::Collider>(physics::GenerationMode::HULL_MODEL);
        obj->add_component<render::ModelComponent>("primitive::cube", red_transparent_material);

        auto obj2 = scene->add_child("obj2", glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.0f), glm::vec3(10.0f, 1.0f, 10.0f));
        obj2->add_component<render::ModelComponent>("primitive::cube", white_material);

        auto plane = scene->add_child("plane", glm::vec3(2.0f, -4.0f, 2.0f), glm::vec3(0.0f), glm::vec3(10.0f, 1.0f, 10.0f));
        plane->add_component<render::ModelComponent>("primitive::plane", white_material);

        auto obj3 = scene->add_child("obj3", glm::vec3(-3.0f, 5.0f, -1.0f), glm::vec3(0.0f));
        obj3->add_component<render::ModelComponent>("primitive::cube", blue_transparent_material);
        obj3->add_component<physics::Collider>(physics::GenerationMode::HULL_MODEL);

        // Spheres
        auto sphere1 = scene->add_child("Sphere1", glm::vec3(-3.0f, 1.0f, -1.0f), glm::vec3(0), glm::vec3(0.5));
        sphere1->add_component<render::ModelComponent>("primitive::uv_sphere:20", nullptr);
        sphere1->add_component<physics::Collider>(physics::GenerationMode::HULL_MODEL);
        sphere1->add_component<TestComponent>();
        sphere1->add_component<MoveUpDown>(0.4, 1, 5);

        // Light
        auto dir_light = scene->add_child("DirLight", glm::vec3(0.2f, 3.0f, 0.0f), glm::vec3(glm::radians(-45.0f), glm::radians(-45.0f), glm::radians(45.0f)));
        dir_light->add_component<render::DirectionalLight>(glm::vec3(0.2f), glm::vec3(0.7f));
        auto point_light = scene->add_child("Point Light", glm::vec3(2.0f, 1.5f, 4.0f));
        point_light->add_component<render::PointLight>(glm::vec3(0.2f), glm::vec3(1.0f), 0.09f, 0.032f);
        point_light->add_component<MoveUpDown>(0.1, 1, 3);
        auto spot_light = scene->add_child("Spotlight", glm::vec3(-6.0f, 3.0f, -2.0f), glm::vec3(glm::radians(-90.0f), 0, 0));
        spot_light->add_component<render::Spotlight>(glm::vec3(0), glm::vec3(1.0f), glm::radians(40.0f), glm::radians(50.0f), 0.09f, 0.032f);

        core::Application::event_bus->subscribe<events::InputKeyEvent>([point_light](const events::InputKeyEvent&) {
            point_light->hidden = !point_light->hidden;
        },
            GLFW_KEY_L);
    }

    core::Application::event_bus->subscribe<events::InputKeyEvent>([&](const events::InputKeyEvent&) {
        auto random = [](float min, float max) {
            float zero_to_one = ((float)std::rand() / (float)RAND_MAX);
            return zero_to_one * (std::abs(min) + std::abs(max)) + min;
        };
        float x = random(-30, 30);
        float y = random(-30, 30);
        float z = random(-30, 30);
        auto new_material = std::make_shared<render::Material>();
        new_material->diffuse_color = glm::vec4(random(0, 1), random(0, 1), random(0, 1), 1.0f);
        auto new_cube = scene->add_child("New Cube", glm::vec3(x, y, z));
        new_cube->add_component<render::ModelComponent>("primitive::cube", new_material);
        tree->sync_scene_tree(scene.get());
        core::Logger::debug("Created cube at ({} {} {})", x, y, z);
    },
        GLFW_KEY_N);

    core::Application::event_bus->subscribe<events::InputKeyEvent>([](const events::InputKeyEvent&) {
        core::Application::option_toggle(core::Option::VSYNC);
    },
        GLFW_KEY_V);

    core::Application::event_bus->subscribe<events::InputKeyEvent>([](const events::InputKeyEvent&) {
        core::Application::option_toggle(core::Option::SHOW_COLLIDERS);
    },
        GLFW_KEY_P);

    scene->start();

    tree->sync_scene_tree(scene.get());

    // Mainloop
    core::Application::mainloop();

    scene->cleanup();
    core::Application::cleanup();

    std::fstream filestream;
    filestream.open("scene.json", std::fstream::out);
    serializer::Serializer::serialize(serializer::GeneratorType::JSON_PRETTY, "scene", scene, filestream);
    filestream.close();

    return 0;
}
