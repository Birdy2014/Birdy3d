#pragma once

#include "ui/Widget.hpp"

namespace Birdy3d {

    class Layout : public Widget {
    public:
        Layout(UIVector pos = UIVector(0_px), UIVector size = UIVector(0_px), Placement placement = Placement::BOTTOM_LEFT, Theme* theme = Application::defaultTheme, std::string name = "");
        void add_child(std::unique_ptr<Widget>);
        template <class T, typename... Args>
        T* add_child(Args... args) {
            static_assert(std::is_base_of<Widget, T>::value);
            std::unique_ptr<Widget> widget = std::make_unique<T>(args...);
            Widget* widget_ptr = widget.get();
            add_child(std::move(widget));
            return static_cast<T*>(widget_ptr);
        }
        void toForeground(Widget* w);
        void draw() override;
        virtual void arrange(glm::vec2 pos, glm::vec2 size) override = 0;
        bool update_hover(bool hover) override;
        virtual void late_update() override;
        void set_canvas(Canvas*) override;

        template <class T>
        T* getWidget(const std::string& name, bool hidden = true) {
            if (this->hidden && !hidden)
                return nullptr;
            for (const std::unique_ptr<Widget>& child : m_children) {
                T* casted = dynamic_cast<T*>(child.get());
                if (casted && child->name == name) {
                    return casted;
                }
                Layout* layout = dynamic_cast<Layout*>(child.get());
                if (layout) {
                    T* result = layout->getWidget<T>(name, hidden);
                    if (result)
                        return result;
                }
            }
            return nullptr;
        }

    protected:
        std::list<std::unique_ptr<Widget>> m_children;

        virtual void on_update() override;
    };

}
