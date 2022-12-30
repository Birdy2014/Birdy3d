#pragma once

#include "Birdy3d.hpp"
#include "core/ResourceHandle.hpp"
#include <filesystem>

template <class T>
class ResourceInput : public Birdy3d::ui::Widget {
public:
    struct Options {
        BIRDY3D_WIDGET_OPTIONS_STRUCT
        Birdy3d::core::ResourceHandle<T>* target;
    };

    ResourceInput(Options options)
        : Widget(options)
        , m_target(options.target)
    {
        using namespace Birdy3d::ui::literals;
        m_text.text(static_cast<std::string>(*options.target));
    }

private:
    Birdy3d::ui::TextDescription m_text;
    Birdy3d::core::ResourceHandle<T>* m_target;

    void draw() override
    {
        auto color_input_bg = Birdy3d::core::Application::theme().color(Birdy3d::utils::Color::Name::BG_INPUT);
        paint_background(color_input_bg);
        paint_text(glm::ivec2(0), m_text);
    }

    void on_drop(Birdy3d::ui::DropEvent& event) override
    {
        if (event.data.type() != typeid(Birdy3d::core::ResourceIdentifier))
            return;
        auto id = std::any_cast<Birdy3d::core::ResourceIdentifier>(event.data);
        *m_target = id;
        m_text.text(static_cast<std::string>(id));
    }
};
