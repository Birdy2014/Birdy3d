#pragma once

#include "Birdy3d.hpp"
#include <filesystem>

class FileBrowser : public Birdy3d::ui::Widget {
public:
    struct Options {
        BIRDY3D_WIDGET_OPTIONS_STRUCT
        std::filesystem::path root_directory;
    };

    FileBrowser(Options options)
        : Birdy3d::ui::Widget(options)
        , m_root_directory(options.root_directory)
        , m_current_directory(options.root_directory) {
        using namespace Birdy3d::ui::literals;
        set_layout<Birdy3d::ui::DirectionalLayout>(Birdy3d::ui::DirectionalLayout::Direction::RIGHT, 10);

        auto tree_scroll_container = add_child<Birdy3d::ui::ScrollContainer>({ .size = { 50_px, 100_p }, .weight = 0.4 });
        tree_scroll_container->set_layout<Birdy3d::ui::MaxLayout>();

        m_tree = tree_scroll_container->add_child<Birdy3d::ui::TreeView>({});
        m_tree->show_root_item = false;
        auto& builtin_root = m_tree->root_item().add_child("builtin");
        builtin_root.data = std::filesystem::path("_builtin");
        auto& builtin_models = builtin_root.add_child("models");
        builtin_models.data = std::filesystem::path("_builtin_models");
        m_root_directory_item = &m_tree->root_item().add_child("");

        m_file_container = add_child<Birdy3d::ui::ScrollContainer>({});
        m_file_container->m_horizontal_scroll_enabled = false;
        m_file_container->set_layout<Birdy3d::ui::DynamicGridLayout>(5);

        m_tree->on_select = [this](Birdy3d::ui::TreeItem& item) {
            if (item.data.type() != typeid(std::filesystem::path))
                return;
            m_current_directory = std::any_cast<std::filesystem::path>(item.data);
            sync();
        };
        sync();
    }

private:
    class FileItem : public Birdy3d::ui::Widget {
    public:
        struct Options {
            BIRDY3D_WIDGET_OPTIONS_STRUCT
            Birdy3d::core::ResourceIdentifier id;
        };

        FileItem(Options options)
            : Birdy3d::ui::Widget(options)
            , m_resource_id(options.id) {
            using namespace Birdy3d::ui::literals;
            add_filled_rectangle(0_px, 100_p, Birdy3d::utils::Color::Name::BG_INPUT);
            m_label = add_text(0_px, std::filesystem::path(options.id.name).filename().string(), Birdy3d::utils::Color::Name::FG, Birdy3d::ui::Placement::BOTTOM_LEFT);
            this->size = { 100 };
        }

    private:
        Birdy3d::core::ResourceIdentifier m_resource_id;
        Birdy3d::ui::Text* m_label;

        void on_click(Birdy3d::ui::ClickEvent& event) override {
            if (event.button == GLFW_MOUSE_BUTTON_LEFT && event.action == GLFW_PRESS)
                canvas->start_drag(m_resource_id);
        }
    };

    std::filesystem::path m_root_directory;
    std::filesystem::path m_current_directory;
    std::shared_ptr<Birdy3d::ui::TreeView> m_tree;
    std::shared_ptr<Birdy3d::ui::ScrollContainer> m_file_container;
    Birdy3d::ui::TreeItem* m_root_directory_item;

    void sync() {
        // Sync directory tree
        m_root_directory_item->text->text(std::filesystem::canonical(m_root_directory).filename().string());
        m_root_directory_item->data = m_root_directory;
        sync(*m_root_directory_item, m_root_directory);

        // Sync files
        m_file_container->clear_children();

        if (m_current_directory.string().starts_with("_builtin")) {
            if (m_current_directory.string() == "_builtin_models") {
                m_file_container->add_child<FileItem>({ .id = "primitive::plane" });
                m_file_container->add_child<FileItem>({ .id = "primitive::cube" });
                m_file_container->add_child<FileItem>({ .id = "primitive::uv_sphere:resolution=20" });
            }
            return;
        }

        for (const auto& entry : std::filesystem::directory_iterator { m_current_directory }) {
            if (entry.is_directory())
                continue;
            m_file_container->add_child<FileItem>({ .id = "file::" + entry.path().string() });
        }
    }

    void sync(Birdy3d::ui::TreeItem& item, std::filesystem::path directory) {
        if (!std::filesystem::is_directory(directory)) {
            item.children.clear();
            return;
        }

        // Remove old TreeItems
        for (const auto& child_item : item.children) {
            if (child_item.data.type() != typeid(std::filesystem::path)) {
                item.remove_child(&child_item);
                continue;
            }
            auto directory_iterator = std::filesystem::directory_iterator { directory };
            auto it = std::find_if(std::filesystem::begin(directory_iterator), std::filesystem::end(directory_iterator), [&](const std::filesystem::path& child_path) {
                return std::filesystem::is_directory(child_path) && child_path == std::any_cast<std::filesystem::path>(child_item.data);
            });
            if (it == std::filesystem::end(directory_iterator)) {
                item.remove_child(&child_item);
            }
        }

        // Add new TreeItems
        for (const auto& child_entry : std::filesystem::directory_iterator { directory }) {
            if (!child_entry.is_directory())
                continue;
            const auto& child_path = child_entry.path();
            auto it = std::find_if(item.children.begin(), item.children.end(), [&](const Birdy3d::ui::TreeItem& child_item) {
                return std::any_cast<std::filesystem::path>(child_item.data) == child_path;
            });
            if (it == item.children.end()) {
                std::string name = child_path.filename().string();
                Birdy3d::ui::TreeItem& new_item = item.add_child(name);
                new_item.data = child_path;
                sync(new_item, child_path);
                continue;
            }
            sync(*it, child_path);
        }
    }
};
