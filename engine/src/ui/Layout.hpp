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

        // FIXME: Rename arrange to do_layout and use Rect instead of pos and size

        /**
         * @brief Sets the position and size of all widgets in the list.
         *
         * The size of the widgets is set so that they match the size given in the size parameter.
         * The size parameter can never exceed the value returned by minimal_size().
         * An overloaded function should call Widget::arrange(glm::ivec2, glm::ivec2) on each child widget.
         */
        virtual void arrange(std::list<std::shared_ptr<Widget>> const& children, glm::ivec2 pos, glm::ivec2 size) const = 0;

        /**
         * @brief Computes the minimal size that the widgets can fit in with the current layout.
         */
        [[nodiscard]] virtual glm::ivec2 minimal_size(std::list<std::shared_ptr<Widget>> const& children) const = 0;

        /**
         * @brief Like minimal_size(const std::list<std::shared_ptr<Widget>>&) const, but with a suggested size in one dimension.
         *
         * This is useful for Layouts that place child widgets
         * differently depending on the width or height.
         */
        [[nodiscard]] virtual glm::ivec2 minimal_size(std::list<std::shared_ptr<Widget>> const& children, float suggested_size, Direction) const;

        /**
         * @brief Get minimal size in a direction if the other dimension is arbitrary.
         *
         * This can be different than minimal_size(const std::list<std::shared_ptr<Widget>>&) const,
         * if the layout changes the placement of widgets depending on its dimensions.
         */
        [[nodiscard]] virtual float minimal_size(std::list<std::shared_ptr<Widget>> const& children, Direction) const;
    };

    /**
     * @brief Fills the available area with the first child widget.
     */
    class MaxLayout : public Layout {
    public:
        void arrange(std::list<std::shared_ptr<Widget>> const& children, glm::ivec2 pos, glm::ivec2 size) const override;
        [[nodiscard]] glm::ivec2 minimal_size(std::list<std::shared_ptr<Widget>> const& children) const override;
    };

    /**
     * @brief Places widgets at their preferred position with their
     * preferred size.
     */
    class AbsoluteLayout : public Layout {
    public:
        void arrange(std::list<std::shared_ptr<Widget>> const& children, glm::ivec2 pos, glm::ivec2 size) const override;
        [[nodiscard]] glm::ivec2 minimal_size(std::list<std::shared_ptr<Widget>> const& children [[maybe_unused]]) const override;
    };

}
