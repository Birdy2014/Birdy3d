#include "Birdy3d.hpp"
#include "FileBrowser.hpp"
#include "ResourceInput.hpp"
#include <filesystem>
#include <fstream>
#ifdef BIRDY3D_PLATFORM_LINUX
    #include <csignal>
#endif

using namespace Birdy3d;

class TestComponent : public ecs::Component {
public:
    void start() override
    {
        core::Application::event_bus->subscribe(this, &TestComponent::on_collision);
    }

    void cleanup() override
    {
        core::Application::event_bus->unsubscribe(this, &TestComponent::on_collision);
    }

    void on_collision(events::CollisionEvent const& event)
    {
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
        , m_limit_up(limit_up)
    { }

    void update() override
    {
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

    void serialize(serializer::Adapter& adapter) override
    {
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
void handler(int sig)
{
    if (sig == SIGSEGV)
        std::cout << "SIGNAL " << sig << "(SIGSEGV)\n";
    else
        std::cout << "SIGNAL " << sig << "\n";

    utils::print_stacktrace();

    exit(1);
}
#endif

int main()
{
#ifdef BIRDY3D_PLATFORM_LINUX
    std::signal(SIGSEGV, handler);
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

    auto menu_bar = canvas->add_child<ui::MenuBar>({.size = ui::Size(100_pc, 1_em), .placement = ui::Placement::TOP_LEFT});
    auto& bar_item1 = menu_bar->add_item("Item 1");
    bar_item1.add_child("Hallo");
    auto& bar_item2 = menu_bar->add_item("Item 2");
    bar_item2.add_child("Welt");

    ui::Console::attach(*canvas);

    auto scene_context_menu = canvas->add_child<ui::ContextMenu>({});

    canvas->add_child<ui::FPSCounter>({.placement = ui::Placement::TOP_RIGHT});

    auto menu = canvas->add_child<ui::Container>({.size = 30_pc, .placement = ui::Placement::CENTER, .name = "menu"});
    menu->set_layout<ui::DirectionalLayout>(ui::DirectionalLayout::Direction::RIGHT, 10);

    auto close_button = menu->add_child<ui::Button>(ui::Button::Options{.placement = ui::Placement::BOTTOM_LEFT, .text = "Close"});
    close_button->callback_click = []() {
        glfwSetWindowShouldClose(core::Application::get_window(), true);
    };

    auto test_button = menu->add_child<ui::Button>({.size = ui::Size(200_px, 50_px), .placement = ui::Placement::BOTTOM_LEFT, .text = "Fenster anzeigen"});

    auto area = menu->add_child<ui::TextField>({.weight = 2});
    area->multiline = true;
    area->append("Hallo Welt\nHallo Welt\naaaaaaaa\naaaaaaa\naaaaaa\naaaaaa");

    auto file_browser_window = canvas->add_child<ui::Window>({.size = ui::Size{500_px, 200_px}});
    file_browser_window->set_layout<ui::MaxLayout>();
    file_browser_window->title("FileBrowser");
    file_browser_window->add_child<FileBrowser>({.root_directory = core::ResourceManager::get_resource_dir()});

    auto tree_window = canvas->add_child<ui::Window>({.size = ui::Size(200_px, 300_px)});
    tree_window->set_layout<ui::MaxLayout>();
    tree_window->title("Scene");

    auto tree_scroll_view = tree_window->add_child<ui::ScrollContainer>({.position = 0_px, .size = 100_pc});
    tree_scroll_view->set_layout<ui::MaxLayout>();

    auto tree = tree_scroll_view->add_child<ui::TreeView>({.size = 100_pc, .placement = ui::Placement::TOP_LEFT});
    tree->m_model = std::make_unique<ui::EntityTreeModel>();
    auto tree_model = static_cast<ui::EntityTreeModel*>(tree->m_model.get());
    tree_model->select_callback = [&](ecs::Entity& entity) {
        core::Application::selected_entity = &entity;
        input_position_x->value(core::Application::selected_entity->transform.position.x);
        input_position_y->value(core::Application::selected_entity->transform.position.y);
        input_position_z->value(core::Application::selected_entity->transform.position.z);
        input_scale_x->value(core::Application::selected_entity->transform.scale.x);
        input_scale_y->value(core::Application::selected_entity->transform.scale.y);
        input_scale_z->value(core::Application::selected_entity->transform.scale.z);
        input_orientation_x->value(glm::degrees(core::Application::selected_entity->transform.orientation.x));
        input_orientation_y->value(glm::degrees(core::Application::selected_entity->transform.orientation.y));
        input_orientation_z->value(glm::degrees(core::Application::selected_entity->transform.orientation.z));

        // Components
        inspector_component_container->clear_children();
        for (auto const& component : core::Application::selected_entity->components()) {
            auto const& c = serializer::Reflector::get_class(component.get());
            auto box = inspector_component_container->add_child<ui::CollapsibleContainer>({.position = ui::Position(0_px, 5_px), .size = ui::Size(100_pc, 0_px), .title = c.name});
            box->set_layout<ui::StaticGridLayout>(5);
            int current_row = 0;
            for (auto const& member : c.m_members) {
                auto label = box->add_child<ui::Label>({.placement = ui::Placement::BOTTOM_LEFT, .column = 0, .row = current_row, .text = member.name});
                auto widget_options = ui::Widget::Options{.size = ui::Size(100_pc, 20_px), .column = 1, .row = current_row};
                if (member.type == typeid(std::string)) {
                    std::weak_ptr<ui::TextField> text_field = box->add_child<ui::TextField>(widget_options);
                    text_field.lock()->text(*(std::string*)member.value);
                    text_field.lock()->on_change = [text_field, member]() {
                        *(std::string*)member.value = text_field.lock()->text();
                    };
                } else if (member.type == typeid(utils::Color)) {
                    std::weak_ptr<ui::TextField> text_field = box->add_child<ui::TextField>(widget_options);
                    text_field.lock()->text(reinterpret_cast<utils::Color*>(member.value)->to_string());
                    text_field.lock()->on_change = [text_field, member]() {
                        *(utils::Color*)member.value = text_field.lock()->text();
                    };
                } else if (member.type == typeid(bool)) {
                    std::weak_ptr<ui::CheckBox> checkbox = box->add_child<ui::CheckBox>(widget_options, {.text = member.name});
                    checkbox.lock()->checked = *(bool*)member.value;
                    checkbox.lock()->on_change = [checkbox, member]() {
                        *(bool*)member.value = checkbox.lock()->checked;
                    };
                } else if (member.type == typeid(int)) {
                    std::weak_ptr<ui::NumberInput> number_input = box->add_child<ui::NumberInput>(widget_options, {});
                    number_input.lock()->value(*(int*)member.value);
                    number_input.lock()->on_change = [number_input, member]() {
                        *(int*)member.value = number_input.lock()->value();
                    };
                } else if (member.type == typeid(float)) {
                    std::weak_ptr<ui::NumberInput> number_input = box->add_child<ui::NumberInput>(widget_options, {});
                    number_input.lock()->value(*(float*)member.value);
                    number_input.lock()->on_change = [number_input, member]() {
                        *(float*)member.value = number_input.lock()->value();
                    };
                } else if (member.type == typeid(core::ResourceHandle<render::Shader>)) {
                    box->add_child<ResourceInput<render::Shader>>(widget_options, {.target = static_cast<core::ResourceHandle<render::Shader>*>(member.value)});
                } else if (member.type == typeid(core::ResourceHandle<ui::Theme>)) {
                    box->add_child<ResourceInput<ui::Theme>>(widget_options, {.target = static_cast<core::ResourceHandle<ui::Theme>*>(member.value)});
                } else if (member.type == typeid(core::ResourceHandle<render::Model>)) {
                    box->add_child<ResourceInput<render::Model>>(widget_options, {.target = static_cast<core::ResourceHandle<render::Model>*>(member.value)});
                } else if (member.type == typeid(core::ResourceHandle<render::Texture>)) {
                    box->add_child<ResourceInput<render::Texture>>(widget_options, {.target = static_cast<core::ResourceHandle<render::Texture>*>(member.value)});
                } else {
                    std::cout << member.name << '\n';
                }
                ++current_row;
            }
        }
    };

    tree_model->select_secundary_callback = [&scene_context_menu](ecs::Entity&) {
        scene_context_menu->open();
    };

    auto& scene_new_menu = scene_context_menu->root_item.add_child("New");
    scene_new_menu.add_child("Empty Entity", [&]() {
        if (core::Application::selected_entity) {
            core::Application::selected_entity->add_child();
            tree->update_cache();
        }
    });
    scene_new_menu.add_child("Plane", [&]() {
        if (core::Application::selected_entity) {
            auto new_entity = core::Application::selected_entity->add_child("Plane");
            new_entity->add_component<render::ModelComponent>("primitive::plane");
            tree->update_cache();
        }
    });
    scene_new_menu.add_child("Cube", [&]() {
        if (core::Application::selected_entity) {
            auto new_entity = core::Application::selected_entity->add_child("Cube");
            new_entity->add_component<render::ModelComponent>("primitive::cube");
            tree->update_cache();
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
            tree->unselect();
            tree->update_cache();
        }
    });

    scene_context_menu->root_item.add_child("Remove", [&]() {
        if (!core::Application::selected_entity || !core::Application::selected_entity->parent)
            return;
        if (auto scene_ptr = core::Application::scene.lock()) {
            core::Application::selected_entity->remove();
            core::Application::selected_entity = nullptr;
            tree->unselect();
            tree->update_cache();
        }
    });

    auto inspector_window = canvas->add_child<ui::Window>({.size = 500_px, .hidden = true});
    inspector_window->set_layout<ui::MaxLayout>();
    inspector_window->title("Inspector");

    inspector_window->callback_close = [&inspector_window]() {
        inspector_window->hidden = !inspector_window->hidden;
    };

    test_button->callback_click = [&inspector_window]() {
        inspector_window->hidden = !inspector_window->hidden;
    };

    auto inspector_scroll_view = inspector_window->add_child<ui::ScrollContainer>({.size = 100_pc, .placement = ui::Placement::BOTTOM_LEFT});
    inspector_scroll_view->set_layout<ui::DirectionalLayout>(ui::DirectionalLayout::Direction::DOWN, 10, true);

    auto transform_box = inspector_scroll_view->add_child<ui::CollapsibleContainer>({.position = ui::Position(0_px, 5_px), .size = ui::Size(100_pc, 0_px), .title = "Transform"});
    transform_box->set_layout<ui::StaticGridLayout>(5);

    auto position_label = transform_box->add_child<ui::Label>({.placement = ui::Placement::CENTER_LEFT, .column = 0, .row = 0, .text = "position"});

    auto position_box = transform_box->add_child<ui::Container>({.size = ui::Size(100_pc, 0_px), .column = 1, .row = 0});
    position_box->set_layout<ui::DirectionalLayout>(ui::DirectionalLayout::Direction::RIGHT, 5, false);
    input_position_x = position_box->add_child<ui::NumberInput>({.size = ui::Size(100_pc, 25_px), .placement = ui::Placement::BOTTOM_LEFT, .value = 0});
    input_position_y = position_box->add_child<ui::NumberInput>({.size = ui::Size(100_pc, 25_px), .placement = ui::Placement::BOTTOM_LEFT, .value = 0});
    input_position_z = position_box->add_child<ui::NumberInput>({.size = ui::Size(100_pc, 25_px), .placement = ui::Placement::BOTTOM_LEFT, .value = 0});

    input_position_x->on_change = [&]() {
        if (core::Application::selected_entity)
            core::Application::selected_entity->transform.position.x = input_position_x->value();
    };

    input_position_y->on_change = [&]() {
        if (core::Application::selected_entity)
            core::Application::selected_entity->transform.position.y = input_position_y->value();
    };

    input_position_z->on_change = [&]() {
        if (core::Application::selected_entity)
            core::Application::selected_entity->transform.position.z = input_position_z->value();
    };

    input_position_x->add_callback("on_focus_lost", [&](ui::UIEvent&) {
        if (core::Application::selected_entity)
            input_position_x->value(core::Application::selected_entity->transform.position.x);
    });

    input_position_y->add_callback("on_focus_lost", [&](ui::UIEvent&) {
        if (core::Application::selected_entity)
            input_position_y->value(core::Application::selected_entity->transform.position.y);
    });

    input_position_z->add_callback("on_focus_lost", [&](ui::UIEvent&) {
        if (core::Application::selected_entity)
            input_position_z->value(core::Application::selected_entity->transform.position.z);
    });

    auto scale_label = transform_box->add_child<ui::Label>({.placement = ui::Placement::CENTER_LEFT, .column = 0, .row = 1, .text = "scale"});

    auto scale_box = transform_box->add_child<ui::Container>({.size = ui::Size(100_pc, 0_px), .column = 1, .row = 1});
    scale_box->set_layout<ui::DirectionalLayout>(ui::DirectionalLayout::Direction::RIGHT, 5, false);
    input_scale_x = scale_box->add_child<ui::NumberInput>({.size = ui::Size(100_pc, 25_px), .value = 0});
    input_scale_y = scale_box->add_child<ui::NumberInput>({.size = ui::Size(100_pc, 25_px), .value = 0});
    input_scale_z = scale_box->add_child<ui::NumberInput>({.size = ui::Size(100_pc, 25_px), .value = 0});

    input_scale_x->min_value = 0;
    input_scale_y->min_value = 0;
    input_scale_z->min_value = 0;

    input_scale_x->on_change = [&]() {
        if (core::Application::selected_entity)
            core::Application::selected_entity->transform.scale.x = input_scale_x->value();
    };

    input_scale_y->on_change = [&]() {
        if (core::Application::selected_entity)
            core::Application::selected_entity->transform.scale.y = input_scale_y->value();
    };

    input_scale_z->on_change = [&]() {
        if (core::Application::selected_entity)
            core::Application::selected_entity->transform.scale.z = input_scale_z->value();
    };

    input_scale_x->add_callback("on_focus_lost", [&](ui::UIEvent&) {
        if (core::Application::selected_entity)
            input_scale_x->value(core::Application::selected_entity->transform.scale.x);
    });

    input_scale_y->add_callback("on_focus_lost", [&](ui::UIEvent&) {
        if (core::Application::selected_entity)
            input_scale_y->value(core::Application::selected_entity->transform.scale.y);
    });

    input_scale_z->add_callback("on_focus_lost", [&](ui::UIEvent&) {
        if (core::Application::selected_entity)
            input_scale_z->value(core::Application::selected_entity->transform.scale.z);
    });

    auto orientation_label = transform_box->add_child<ui::Label>({.placement = ui::Placement::CENTER_LEFT, .column = 0, .row = 2, .text = "orientation"});

    auto orientation_box = transform_box->add_child<ui::Container>({.size = ui::Size(100_pc, 0_px), .column = 1, .row = 2});
    orientation_box->set_layout<ui::DirectionalLayout>(ui::DirectionalLayout::Direction::RIGHT, 5, false);
    input_orientation_x = orientation_box->add_child<ui::NumberInput>({.size = ui::Size(100_pc, 25_px), .placement = ui::Placement::BOTTOM_LEFT, .value = 0});
    input_orientation_y = orientation_box->add_child<ui::NumberInput>({.size = ui::Size(100_pc, 25_px), .placement = ui::Placement::BOTTOM_LEFT, .value = 0});
    input_orientation_z = orientation_box->add_child<ui::NumberInput>({.size = ui::Size(100_pc, 25_px), .placement = ui::Placement::BOTTOM_LEFT, .value = 0});

    input_orientation_x->on_change = [&]() {
        if (core::Application::selected_entity)
            core::Application::selected_entity->transform.orientation.x = glm::radians(input_orientation_x->value());
    };

    input_orientation_y->on_change = [&]() {
        if (core::Application::selected_entity)
            core::Application::selected_entity->transform.orientation.y = glm::radians(input_orientation_y->value());
    };

    input_orientation_z->on_change = [&]() {
        if (core::Application::selected_entity)
            core::Application::selected_entity->transform.orientation.z = glm::radians(input_orientation_z->value());
    };

    input_orientation_x->add_callback("on_focus_lost", [&](std::any) {
        if (core::Application::selected_entity)
            input_orientation_x->value(glm::degrees(core::Application::selected_entity->transform.orientation.x));
    });

    input_orientation_y->add_callback("on_focus_lost", [&](std::any) {
        if (core::Application::selected_entity)
            input_orientation_y->value(glm::degrees(core::Application::selected_entity->transform.orientation.y));
    });

    input_orientation_z->add_callback("on_focus_lost", [&](std::any) {
        if (core::Application::selected_entity)
            input_orientation_z->value(glm::degrees(core::Application::selected_entity->transform.orientation.z));
    });

    core::Application::event_bus->subscribe<events::TransformChangedEvent>([&](events::TransformChangedEvent const& event) {
        if (event.entity != core::Application::selected_entity)
            return;
        if (!input_position_x->is_focused())
            input_position_x->value(event.entity->transform.position.x);
        if (!input_position_y->is_focused())
            input_position_y->value(event.entity->transform.position.y);
        if (!input_position_z->is_focused())
            input_position_z->value(event.entity->transform.position.z);
        if (!input_scale_x->is_focused())
            input_scale_x->value(event.entity->transform.scale.x);
        if (!input_scale_y->is_focused())
            input_scale_y->value(event.entity->transform.scale.y);
        if (!input_scale_z->is_focused())
            input_scale_z->value(event.entity->transform.scale.z);
        if (!input_orientation_x->is_focused())
            input_orientation_x->value(glm::degrees(event.entity->transform.orientation.x));
        if (!input_orientation_y->is_focused())
            input_orientation_y->value(glm::degrees(event.entity->transform.orientation.y));
        if (!input_orientation_z->is_focused())
            input_orientation_z->value(glm::degrees(event.entity->transform.orientation.z));
    });

    inspector_component_container = inspector_scroll_view->add_child<ui::Container>({.size = ui::Size(100_pc, 0_px)});
    inspector_component_container->set_layout<ui::DirectionalLayout>(ui::DirectionalLayout::Direction::DOWN, 10, true);

    // Entities
    std::shared_ptr<ecs::Scene> scene;
    auto scene_path = core::ResourceManager::get_resource_dir() + "scene.json";
    if (std::filesystem::exists(scene_path)) {
        serializer::JsonParser parser(core::ResourceManager::read_file(scene_path));
        serializer::Serializer::deserialize(core::ResourceManager::read_file(scene_path), "scene", scene);
        core::Application::scene = scene;
    } else {
        scene = std::make_shared<ecs::Scene>("Scene");
        core::Application::scene = scene;

        auto player = scene->add_child("Player", glm::vec3(0, 0, 3));
        scene->main_camera = player->add_component<render::Camera>(render::Rendertarget::DEFAULT, true);
        auto player_controller = player->add_component<utils::FPPlayerController>();

        auto flashlight = player->add_child("Flashlight", glm::vec3(0), glm::vec3(0));
        flashlight->add_component<render::Spotlight>(utils::Color::WHITE, 0.0f, 0.8f, 0.08f, 0.02f, glm::radians(30.0f), glm::radians(40.0f), false);
        flashlight->hidden = true;

        player_controller->flashlight = flashlight;

        auto white_material = std::make_shared<render::Material>();
        white_material->specular_value = 1.0f;
        auto red_transparent_material = std::make_shared<render::Material>();
        red_transparent_material->diffuse_color = glm::vec4(1.0f, 0.0f, 1.0f, 0.5f);
        auto blue_transparent_material = std::make_shared<render::Material>();
        blue_transparent_material->diffuse_color = glm::vec4(0.0f, 1.0f, 1.0f, 0.5f);

        auto obj = scene->add_child("obj", glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f));
        obj->add_component<physics::ColliderComponent>(physics::GenerationMode::HULL_MODEL);
        obj->add_component<render::ModelComponent>("primitive::cube", red_transparent_material);

        auto obj2 = scene->add_child("obj2", glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.0f), glm::vec3(10.0f, 1.0f, 10.0f));
        obj2->add_component<render::ModelComponent>("primitive::cube", white_material);

        auto plane = scene->add_child("plane", glm::vec3(2.0f, -4.0f, 2.0f), glm::vec3(0.0f), glm::vec3(10.0f, 1.0f, 10.0f));
        plane->add_component<render::ModelComponent>("primitive::plane", white_material);

        auto obj3 = scene->add_child("obj3", glm::vec3(-3.0f, 5.0f, -1.0f), glm::vec3(0.0f));
        obj3->add_component<render::ModelComponent>("primitive::cube", blue_transparent_material);
        obj3->add_component<physics::ColliderComponent>(physics::GenerationMode::HULL_MODEL);

        // Spheres
        auto sphere1 = scene->add_child("Sphere1", glm::vec3(-3.0f, 1.0f, -1.0f), glm::vec3(0), glm::vec3(0.5));
        sphere1->add_component<render::ModelComponent>("primitive::uv_sphere:resolution=20", nullptr);
        sphere1->add_component<physics::ColliderComponent>(physics::GenerationMode::HULL_MODEL);
        sphere1->add_component<TestComponent>();
        sphere1->add_component<MoveUpDown>(0.4, 1, 5);

        // Light
        auto dir_light = scene->add_child("DirLight", glm::vec3(0.2f, 3.0f, 0.0f), glm::vec3(glm::radians(-45.0f), glm::radians(-45.0f), glm::radians(45.0f)));
        dir_light->add_component<render::DirectionalLight>(utils::Color::WHITE, 0.1f, 0.6f);
        auto point_light = scene->add_child("Point Light", glm::vec3(2.0f, 1.5f, 4.0f));
        point_light->add_component<render::PointLight>(utils::Color::WHITE, 0.2f, 0.9f, 0.09f, 0.032f);
        point_light->add_component<MoveUpDown>(0.1, 1, 3);
        auto spot_light = scene->add_child("Spotlight", glm::vec3(-6.0f, 3.0f, -2.0f), glm::vec3(glm::radians(-90.0f), 0, 0));
        spot_light->add_component<render::Spotlight>("#ee9955", 0.0f, 1.0f, 0.09f, 0.032f, glm::radians(40.0f), glm::radians(50.0f));

        core::Application::event_bus->subscribe<events::InputKeyEvent>([point_light](events::InputKeyEvent const&) {
            point_light->hidden = !point_light->hidden;
        },
            GLFW_KEY_L);
    }

    core::Application::event_bus->subscribe<events::InputKeyEvent>([&](events::InputKeyEvent const&) {
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
        tree->update_cache();
        core::Logger::debug("Created cube at ({} {} {})", x, y, z);
    },
        GLFW_KEY_N);

    core::Application::event_bus->subscribe<events::InputKeyEvent>([](events::InputKeyEvent const&) {
        core::Application::option_toggle(core::BoolOption::VSYNC);
    },
        GLFW_KEY_V);

    core::Application::event_bus->subscribe<events::InputKeyEvent>([](events::InputKeyEvent const&) {
        core::Application::option_toggle(core::BoolOption::SHOW_COLLIDERS);
    },
        GLFW_KEY_P);

    scene->start();

    tree_model->root_entity = scene;
    tree->update_cache();

    // Mainloop
    core::Application::mainloop();

    scene->cleanup();
    core::Application::cleanup();

    std::fstream filestream;
    filestream.open(scene_path, std::fstream::out);
    serializer::Serializer::serialize(serializer::GeneratorType::JSON_PRETTY, "scene", scene, filestream);
    filestream.close();

    return 0;
}
