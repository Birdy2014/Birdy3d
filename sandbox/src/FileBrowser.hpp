#pragma once

#include "Birdy3d.hpp"
#include <filesystem>

using namespace Birdy3d;

class FileBrowserTreeModel : public ui::TreeModel {
public:
    std::function<void(core::ResourceIdentifier)> select_callback;
    std::function<void(core::ResourceIdentifier)> select_secundary_callback;

    core::ResourceIdentifier root_directory;

    FileBrowserTreeModel() = default;

    virtual std::optional<ui::TreeItem> item(ui::TreeItem const* parent, std::size_t local_index) override;
    virtual void move_item(ui::TreeItem const& source, ui::TreeItem const& target_parent, std::size_t target_index) override;
    virtual void on_select(int button, ui::TreeItem const&) override;
};

class FileBrowser : public ui::Widget {
public:
    struct Options {
        BIRDY3D_WIDGET_OPTIONS_STRUCT
        std::filesystem::path root_directory;
    };

    FileBrowser(Options options)
        : ui::Widget(options)
        , m_root_directory(options.root_directory.string())
        , m_current_directory(options.root_directory.string()) {
        using namespace ui::literals;
        set_layout<ui::DirectionalLayout>(ui::DirectionalLayout::Direction::RIGHT, 10);

        auto tree_scroll_container = add_child<ui::ScrollContainer>({ .size = { 50_px, 100_pc }, .weight = 0.4 });
        tree_scroll_container->set_layout<ui::MaxLayout>();

        m_tree = tree_scroll_container->add_child<ui::TreeView>({});
        m_tree->m_model = std::make_unique<FileBrowserTreeModel>();
        auto tree_model = static_cast<FileBrowserTreeModel*>(m_tree->m_model.get());
        m_root_directory.source = "file";
        m_root_directory.name = options.root_directory.string();
        if (m_root_directory.name.ends_with('/'))
            m_root_directory.name = m_root_directory.name.substr(0, m_root_directory.name.length() - 1);
        tree_model->root_directory = m_root_directory;

        m_file_container = add_child<ui::ScrollContainer>({});
        m_file_container->m_horizontal_scroll_enabled = false;
        m_file_container->set_layout<ui::DynamicGridLayout>(5);

        tree_model->select_callback = [this](core::ResourceIdentifier item) {
            m_current_directory = item;
            sync();
        };
        sync();
    }

private:
    class FileItem : public ui::Widget {
    public:
        struct Options {
            BIRDY3D_WIDGET_OPTIONS_STRUCT
            core::ResourceIdentifier id;
        };

        FileItem(Options options)
            : ui::Widget(options)
            , m_resource_id(options.id) {
            using namespace ui::literals;
            add_filled_rectangle(0_px, 100_pc, utils::Color::Name::BG_INPUT);
            m_label = add_text(0_px, std::filesystem::path(options.id.name).filename().string(), utils::Color::Name::FG, ui::Placement::BOTTOM_LEFT);
            this->size = 100_px;
        }

    private:
        core::ResourceIdentifier m_resource_id;
        ui::Text* m_label;

        void on_click(ui::ClickEvent& event) override {
            if (event.button == GLFW_MOUSE_BUTTON_LEFT && event.action == GLFW_PRESS)
                canvas->start_drag(m_resource_id);
        }
    };

    core::ResourceIdentifier m_root_directory;
    core::ResourceIdentifier m_current_directory;
    std::shared_ptr<ui::TreeView> m_tree;
    std::shared_ptr<ui::ScrollContainer> m_file_container;

    void sync();
};
