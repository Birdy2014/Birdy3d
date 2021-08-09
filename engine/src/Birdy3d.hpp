#pragma once

// Core
#include "core/Application.hpp"
#include "core/FPPlayerController.hpp"
#include "core/Input.hpp"
#include "core/Logger.hpp"

// Events
#include "events/CollisionEvent.hpp"
#include "events/Event.hpp"
#include "events/EventBus.hpp"
#include "events/InputEvents.hpp"
#include "events/TransformChangedEvent.hpp"
#include "events/WindowResizeEvent.hpp"

// Physics
#include "physics/Collider.hpp"
#include "physics/ConvexMeshGenerators.hpp"
#include "physics/PhysicsWorld.hpp"

// Renderer
#include "render/Camera.hpp"
#include "render/Color.hpp"
#include "render/DirectionalLight.hpp"
#include "render/Material.hpp"
#include "render/Model.hpp"
#include "render/ModelComponent.hpp"
#include "render/PointLight.hpp"
#include "render/Shader.hpp"
#include "render/Spotlight.hpp"
#include "render/Texture.hpp"

// Scene
#include "scene/Component.hpp"
#include "scene/GameObject.hpp"
#include "scene/Scene.hpp"
#include "scene/Transform.hpp"

// UI
#include "ui/AbsoluteLayout.hpp"
#include "ui/Canvas.hpp"
#include "ui/DirectionalLayout.hpp"
#include "ui/Layout.hpp"
#include "ui/MaxLayout.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"
#include "ui/Utils.hpp"
#include "ui/Widget.hpp"
#include "ui/Window.hpp"
#include "ui/widgets/Button.hpp"
#include "ui/widgets/CheckBox.hpp"
#include "ui/widgets/ContextMenu.hpp"
#include "ui/widgets/FPSCounter.hpp"
#include "ui/widgets/NumberInput.hpp"
#include "ui/widgets/TextField.hpp"
#include "ui/widgets/Textarea.hpp"
#include "ui/widgets/TreeView.hpp"
#include "ui/widgets/WindowSnapArea.hpp"
