#pragma once

#include "ui/Widget.hpp"

namespace Birdy3d {

    class Layout : public Widget {
    public:
        Layout(UIVector pos = UIVector(0), UIVector size = UIVector(0), Placement placement = Placement::BOTTOM_LEFT);
        void addChild(Widget* w);
        void draw() override;
        virtual void arrange(glm::mat4 move, glm::vec2 size) override = 0;
        bool updateEvents(bool hidden = false) override;

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
    };

}
