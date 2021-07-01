#include "ui/Widget.hpp"

#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"

namespace Birdy3d {

    class FPSCounter : public Widget {
    public:
        Text* text;

        FPSCounter(UIVector position, Placement placement)
            : Widget(position, 0_px, placement) {
            addText(0_px, 15, "FPS: ", Color::WHITE, Placement::CENTER_LEFT);
            text = getShape<Text>();
        }

        void draw() override {
            int fps = 1 / Application::deltaTime;
            text->text = "FPS: " + std::to_string(fps);
            Widget::draw();
        }

        glm::vec2 minimalSize() override {
            glm::vec2 minSize = theme->text_renderer()->textSize("FPS: 000", text->fontSize) + 2.0f;
            return glm::max(minSize, Widget::minimalSize());
        }
    };

}
