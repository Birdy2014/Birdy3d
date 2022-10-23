#pragma once

#include <glm/glm.hpp>
#include <list>
#include <memory>

namespace Birdy3d::ui {

    class Widget;

    class Layout {
    public:
        enum class Direction {
            HORIZONTAL,
            VERTICAL
        };

        virtual ~Layout() = default;

        /**
         * @brief Sets the position and size of all widgets in the list.
         *
         * The size of the widgets is set so that they match the size given in the size parameter.
         * The size parameter can never exceed the value returned by minimal_size().
         * An overloaded function should call Widget::arrange(glm::ivec2, glm::ivec2) on each child widget.
         */
        virtual void arrange(const std::list<std::shared_ptr<Widget>>& children, glm::ivec2 pos, glm::ivec2 size) const = 0;

        /**
         * @brief Computes the minimal size that the widgets can fit in with the current layout.
         */
        virtual glm::ivec2 minimal_size(const std::list<std::shared_ptr<Widget>>& children) const = 0;

        /**
         * @brief Like minimal_size(const std::list<std::shared_ptr<Widget>>&) const, but with a suggested size in one dimension.
         *
         * This is useful for Layouts that place child widgets
         * differently depending on the width or height.
         */
        virtual glm::ivec2 minimal_size(const std::list<std::shared_ptr<Widget>>& children, float suggested_size, Direction) const;

        /**
         * @brief Get minimal size in a direction if the other dimension is arbitrary.
         *
         * This can be different than minimal_size(const std::list<std::shared_ptr<Widget>>&) const,
         * if the layout changes the placement of widgets depending on its dimensions.
         */
        virtual float minimal_size(const std::list<std::shared_ptr<Widget>>& children, Direction) const;
    };

    /**
     * @brief Fills the available area with the first child widget.
     */
    class MaxLayout : public Layout {
    public:
        void arrange(const std::list<std::shared_ptr<Widget>>& children, glm::ivec2 pos, glm::ivec2 size) const override;
        glm::ivec2 minimal_size(const std::list<std::shared_ptr<Widget>>& children) const override;
    };

    /**
     * @brief Places widgets at their preferred position with their
     * preferred size.
     */
    class AbsoluteLayout : public Layout {
    public:
        void arrange(const std::list<std::shared_ptr<Widget>>& children, glm::ivec2 pos, glm::ivec2 size) const override;
        glm::ivec2 minimal_size(const std::list<std::shared_ptr<Widget>>& children [[maybe_unused]]) const override;
    };

}
