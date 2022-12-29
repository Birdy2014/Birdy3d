#include "ui/Painter.hpp"

#include "ui/OpenGLPainter.hpp"

namespace Birdy3d::ui {

    std::unique_ptr<Painter> Painter::m_instance;

    Painter& Painter::the()
    {
        if (!m_instance) {
            m_instance = std::make_unique<OpenGLPainter>();
        }
        return *m_instance;
    }

}
