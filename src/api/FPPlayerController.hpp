#ifndef BIRDY3D_FPPLAYERCONTROLLER_HPP
#define BIRDY3D_FPPLAYERCONTROLLER_HPP

#include "Component.hpp"
#include "../render/Camera.hpp"

class FPPlayerController : public Component {
public:
    FPPlayerController();
    void start() override;
    void update(float deltaTime) override;

private:
    Camera *cam;
    bool hiddenStatusUpdated = false;
};

#endif
