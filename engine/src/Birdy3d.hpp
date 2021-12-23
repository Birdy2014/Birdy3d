#pragma once

// Core
#include "core/Application.hpp"
#include "core/Input.hpp"
#include "core/Logger.hpp"
#include "core/ResourceManager.hpp"

// Entity Component System
#include "ecs/Component.hpp"
#include "ecs/Entity.hpp"
#include "ecs/Scene.hpp"
#include "ecs/Transform.hpp"

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
#include "render/DirectionalLight.hpp"
#include "render/Material.hpp"
#include "render/Model.hpp"
#include "render/ModelComponent.hpp"
#include "render/PointLight.hpp"
#include "render/Shader.hpp"
#include "render/Spotlight.hpp"
#include "render/Texture.hpp"

// UI
#include "ui/AbsoluteLayout.hpp"
#include "ui/Canvas.hpp"
#include "ui/DirectionalLayout.hpp"
#include "ui/GridLayout.hpp"
#include "ui/Layout.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Theme.hpp"
#include "ui/Units.hpp"
#include "ui/Widget.hpp"
#include "ui/Window.hpp"
#include "ui/console/Console.hpp"
#include "ui/widgets/Button.hpp"
#include "ui/widgets/CheckBox.hpp"
#include "ui/widgets/CollapsibleBox.hpp"
#include "ui/widgets/ContextMenu.hpp"
#include "ui/widgets/FPSCounter.hpp"
#include "ui/widgets/NumberInput.hpp"
#include "ui/widgets/ScrollView.hpp"
#include "ui/widgets/TextField.hpp"
#include "ui/widgets/Textarea.hpp"
#include "ui/widgets/TreeView.hpp"
#include "ui/widgets/WindowSnapArea.hpp"

// Utils
#include "utils/Color.hpp"
#include "utils/FPPlayerController.hpp"
#include "utils/PrimitiveGenerator.hpp"
#include "utils/serializer/Reflector.hpp"
#include "utils/serializer/Serializer.hpp"

// Namespaces
namespace Birdy3d {

    using namespace ui::literals;

}
