#pragma once

#include "ui/Widget.hpp"

namespace Birdy3d {

    class Layout : public Widget {
    public:
        Layout(UIVector pos = UIVector(0_px), UIVector size = UIVector(0_px), Placement placement = Placement::BOTTOM_LEFT, Theme* theme = Application::defaultTheme, std::string name = "");
        void addChild(Widget* w);
        void draw() override;
        virtual void arrange(glm::vec2 pos, glm::vec2 size) override = 0;

        template <class T>
        T* getWidget(const std::string& name, bool hidden = true) {
            if (this->hidden && !hidden)
                return nullptr;
            for (Widget* child : this->children) {
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
        std::vector<Widget*> children;

        virtual void lateUpdate();

        // Events
        virtual bool update(bool hover) override;
        virtual bool onScroll(InputScrollEvent* event, bool hover) override;
        virtual bool onClick(InputClickEvent* event, bool hover) override;
        virtual bool onKey(InputKeyEvent* event, bool hover) override;
        virtual bool onChar(InputCharEvent* event, bool hover) override;
    };

}
