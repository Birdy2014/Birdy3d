#include "FileBrowser.hpp"

std::optional<ui::TreeItem> FileBrowserTreeModel::item(ui::TreeItem const* parent, std::size_t local_index)
{
    // Root level
    if (!parent) {
        switch (local_index) {
        case 0:
            return ui::TreeItem{
                .text = "builtin",
                .data = std::make_any<core::ResourceIdentifier>("primitive::"),
                .local_index = local_index,
                .is_leaf = false,
                .hash = std::hash<std::string>{}("primitive::")};
        case 1:
            return ui::TreeItem{
                .text = std::filesystem::path{root_directory.name}.filename().string(),
                .data = std::make_any<core::ResourceIdentifier>(root_directory),
                .local_index = local_index,
                .is_leaf = !std::filesystem::is_directory(root_directory.name),
                .hash = std::hash<std::string>{}(root_directory.name)};
        default:
            return {};
        }
    }

    auto parent_identifier = std::any_cast<core::ResourceIdentifier>(parent->data);

    // Primitive section
    if (parent_identifier.source == "primitive") {
        if (!parent_identifier.name.empty())
            return {};

        core::ResourceIdentifier id;

        switch (local_index) {
        case 0:
            id.source = "primitive";
            id.type = core::ResourceType::MODEL;
            return ui::TreeItem{
                .text = "models",
                .data = std::make_any<core::ResourceIdentifier>(id),
                .local_index = local_index,
                .is_leaf = true,
                .hash = std::hash<std::string>{}("primitive::") + 1 // FIXME: hash ResourceIdentifier with type
            };
        default:
            return {};
        }
    }

    // File section
    if (parent_identifier.source != "file")
        return {};

    auto parent_directory = std::filesystem::path{parent_identifier.name};

    if (!std::filesystem::is_directory(parent_directory))
        return {};

    std::filesystem::directory_iterator directory_iterator{parent_directory};

    std::size_t current_index = 0;
    for (auto entry : directory_iterator) {
        if (!entry.is_directory())
            continue;

        bool is_leaf = true;
        for (auto child_entry : std::filesystem::directory_iterator{entry}) {
            if (child_entry.is_directory()) {
                is_leaf = false;
                break;
            }
        }

        if (current_index++ == local_index) {
            core::ResourceIdentifier id;
            id.source = "file";
            id.name = entry.path().string();
            return ui::TreeItem{
                .text = entry.path().filename().string(),
                .data = std::make_any<core::ResourceIdentifier>(id),
                .local_index = local_index,
                .is_leaf = is_leaf,
                .hash = std::filesystem::hash_value(entry.path())};
        }
    }

    return {};
}

void FileBrowserTreeModel::move_item([[maybe_unused]] ui::TreeItem const& source, [[maybe_unused]] ui::TreeItem const& target_parent, [[maybe_unused]] std::size_t target_index)
{
    return;
}

void FileBrowserTreeModel::on_select(int button, ui::TreeItem const& item)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && select_callback)
        std::invoke(select_callback, std::any_cast<core::ResourceIdentifier>(item.data));
    if (button == GLFW_MOUSE_BUTTON_RIGHT && select_secundary_callback)
        std::invoke(select_secundary_callback, std::any_cast<core::ResourceIdentifier>(item.data));
}

void FileBrowser::sync()
{
    // Sync files
    m_file_container->clear_children();

    if (m_current_directory.source == "primitive") {
        if (m_current_directory.type == core::ResourceType::MODEL) {
            m_file_container->add_child<FileItem>({.id = "primitive::plane"});
            m_file_container->add_child<FileItem>({.id = "primitive::cube"});
            m_file_container->add_child<FileItem>({.id = "primitive::uv_sphere:resolution=20"});
        }
        return;
    }

    if (!std::filesystem::is_directory(m_current_directory.name))
        return;

    for (auto const& entry : std::filesystem::directory_iterator{m_current_directory.name}) {
        if (entry.is_directory())
            continue;
        m_file_container->add_child<FileItem>({.id = "file::" + entry.path().string()});
    }
}
