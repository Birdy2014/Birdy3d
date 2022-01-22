#pragma once

#include "Birdy3d.hpp"
#include <filesystem>

class FileBrowser : public Birdy3d::ui::Widget {
public:
    FileBrowser(Birdy3d::ui::Widget::Options options, std::filesystem::path root_directory)
        : Birdy3d::ui::Widget(options)
        , m_root_directory(root_directory)
        , m_current_directory(root_directory) {
        using namespace Birdy3d::ui::literals;
        set_layout<Birdy3d::ui::DirectionalLayout>(Birdy3d::ui::DirectionalLayout::Direction::RIGHT, true);
        m_tree = add_child<Birdy3d::ui::TreeView>({ .size = { 50_px, 100_p }, .weight = 0.4 });
        m_file_container = add_child<Birdy3d::ui::Container>({});
        m_file_container->set_layout<Birdy3d::ui::DynamicGridLayout>(5);
        m_tree->add_callback("select", [this](std::any data) {
            if (data.type() != typeid(Birdy3d::ui::TreeItem*))
                return;
            auto item = std::any_cast<Birdy3d::ui::TreeItem*>(data);
            if (item->data.type() != typeid(std::filesystem::path))
                return;
            m_current_directory = std::any_cast<std::filesystem::path>(item->data);
            sync();
        });
        sync();
    }

private:
    class FileItem : public Birdy3d::ui::Widget {
    public:
        FileItem(Birdy3d::ui::Widget::Options options, std::filesystem::path path)
            : Birdy3d::ui::Widget(options)
            , m_path(path) {
            using namespace Birdy3d::ui::literals;
            add_filled_rectangle(0_px, 100_p, Birdy3d::utils::Color::Name::BG_INPUT);
            m_label = add_text(0_px, path.filename(), Birdy3d::utils::Color::Name::FG, Birdy3d::ui::Placement::BOTTOM_LEFT);
            this->options.size = { 100 };
        }

    private:
        std::filesystem::path m_path;
        Birdy3d::ui::Text* m_label;

        void on_click(const Birdy3d::events::InputClickEvent& event) override {
        }
    };

    std::filesystem::path m_root_directory;
    std::filesystem::path m_current_directory;
    std::shared_ptr<Birdy3d::ui::TreeView> m_tree;
    std::shared_ptr<Birdy3d::ui::Container> m_file_container;

    void sync() {
        // Sync directory tree
        m_tree->root_item().text->text(std::filesystem::canonical(m_root_directory).filename());
        m_tree->root_item().data = m_root_directory;
        sync(m_tree->root_item(), m_root_directory);

        // Sync files
        m_file_container->clear_children();
        for (const auto& entry : std::filesystem::directory_iterator { m_current_directory }) {
            if (entry.is_directory())
                continue;
            m_file_container->add_child<FileItem>({}, entry.path());
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
                std::string name = child_path.filename();
                Birdy3d::ui::TreeItem& new_item = item.add_child(name);
                new_item.data = child_path;
                sync(new_item, child_path);
                continue;
            }
            sync(*it, child_path);
        }
    }
};
