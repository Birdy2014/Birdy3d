#pragma once

#include <glm/glm.hpp>
#include <list>
#include <memory>

namespace Birdy3d::ui {

    class Widget;

    class Layout {
    public:
        virtual ~Layout() = default;

        /**
         * @brief Sets the position and size of all widgets in the list.
         *
         * The size of the widgets is set so that they match the size given in the size parameter.
         * The size parameter can never exceed the value returned by minimal_size().
         * An overloaded function should call Widget::arrange(glm::vec2, glm::vec2) on each child widget.
         */
        virtual void arrange(const std::list<std::shared_ptr<Widget>>& children, glm::vec2 pos, glm::vec2 size) const = 0;

        /**
         * @brief Computes the minimal size that the widgets can fit in with the current layout.
         */
        virtual glm::vec2 minimal_size(const std::list<std::shared_ptr<Widget>>& children) const = 0;
    };

    /**
     * @brief Fills the available area with the first child widget.
     */
    class MaxLayout : public Layout {
    public:
        void arrange(const std::list<std::shared_ptr<Widget>>& children, glm::vec2 pos, glm::vec2 size) const override;
        glm::vec2 minimal_size(const std::list<std::shared_ptr<Widget>>& children) const override;
    };

}
