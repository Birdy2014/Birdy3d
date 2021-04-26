#pragma once

// Core
#include "core/Application.hpp"
#include "core/Component.hpp"
#include "core/FPPlayerController.hpp"
#include "core/GameObject.hpp"
#include "core/Input.hpp"
#include "core/Logger.hpp"
#include "core/Transform.hpp"

// Events
#include "events/CollisionEvent.hpp"
#include "events/Event.hpp"
#include "events/EventBus.hpp"
#include "events/InputEvents.hpp"
#include "events/WindowResizeEvent.hpp"

// Physics
#include "physics/Collider.hpp"
#include "physics/CollisionMesh.hpp"
#include "physics/CollisionSphere.hpp"
#include "physics/PhysicsWorld.hpp"

// Renderer
#include "render/Camera.hpp"
#include "render/DirectionalLight.hpp"
#include "render/Model.hpp"
#include "render/PointLight.hpp"
#include "render/Shader.hpp"
#include "render/Spotlight.hpp"
#include "render/Texture.hpp"

// UI
#include "ui/Canvas.hpp"
#include "ui/Layout.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"
#include "ui/Utils.hpp"
#include "ui/Widget.hpp"
#include "ui/widgets/Button.hpp"
#include "ui/widgets/DirectionalLayout.hpp"
#include "ui/widgets/FPSCounter.hpp"
#include "ui/widgets/Textarea.hpp"
