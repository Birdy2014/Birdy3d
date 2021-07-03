#pragma once

#include "ui/Widget.hpp"

namespace Birdy3d {

    class Layout : public Widget {
    public:
        Layout(UIVector pos = UIVector(0_px), UIVector size = UIVector(0_px), Placement placement = Placement::BOTTOM_LEFT, Theme* theme = Application::defaultTheme, std::string name = "");
        void addChild(Widget* w);
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
            for (Widget* child : m_children) {
                T* casted = dynamic_cast<T*>(child);
                if (casted && child->name == name) {
                    return casted;
                }
                Layout* layout = dynamic_cast<Layout*>(child);
                if (layout) {
                    T* result = layout->getWidget<T>(name, hidden);
                    if (result)
                        return result;
                }
            }
            return nullptr;
        }

    protected:
        std::list<Widget*> m_children;

        virtual void on_update() override;
    };

}
