#pragma once

#include "Birdy3d.hpp"
#include "core/ResourceHandle.hpp"
#include <filesystem>

template <class T>
class ResourceInput : public Birdy3d::ui::Widget {
public:
    ResourceInput(Birdy3d::ui::Widget::Options options, Birdy3d::core::ResourceHandle<T>* target)
        : Widget(options)
        , m_target(target) {
        using namespace Birdy3d::ui::literals;
        add_filled_rectangle(0_px, 100_p, Birdy3d::utils::Color::Name::BG_INPUT);
        m_text = add_text(0_px, static_cast<std::string>(*target), Birdy3d::utils::Color::Name::FG);
    }

private:
    Birdy3d::ui::Text* m_text;
    Birdy3d::core::ResourceHandle<T>* m_target;

    void on_drop(const std::any& value) override {
        if (value.type() != typeid(Birdy3d::core::ResourceIdentifier))
            return;
        auto id = std::any_cast<Birdy3d::core::ResourceIdentifier>(value);
        *m_target = id;
        *m_text = static_cast<std::string>(id);
    }
};
