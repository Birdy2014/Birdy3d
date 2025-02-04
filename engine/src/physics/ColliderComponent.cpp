#include "physics/ColliderComponent.hpp"

#include "ecs/Entity.hpp"
#include "events/ResourceEvents.hpp"
#include "render/ModelComponent.hpp"

namespace Birdy3d::physics {

    ColliderComponent::ColliderComponent() { }

    ColliderComponent::ColliderComponent(GenerationMode generation_mode)
        : m_generation_mode(generation_mode)
    { }

    void ColliderComponent::start()
    {
        core::Application::event_bus->subscribe(this, &ColliderComponent::on_resource_loaded);
        reload_collider();
    }

    void ColliderComponent::cleanup()
    {
        core::Application::event_bus->unsubscribe(this, &ColliderComponent::on_resource_loaded);
    }

    void ColliderComponent::on_resource_loaded(events::ResourceLoadEvent const&)
    {
        reload_collider();
    }

    void ColliderComponent::reload_collider()
    {
        auto model_component = entity->get_component<render::ModelComponent>();
        if (!model_component) {
            core::Logger::warn("Entity '{}' doesn't have any ModelComponent", entity->name);
            return;
        }

        auto model = model_component->model();
        if (!model) {
            core::Logger::warn("Entity '{}' doesn't have any model", entity->name);
            return;
        }

        auto collider_id = model.id();
        switch (m_generation_mode) {
        case GenerationMode::NONE:
            collider_id.args["generation_mode"] = "NONE";
            break;
        case GenerationMode::COPY:
            collider_id.args["generation_mode"] = "COPY";
            break;
        case GenerationMode::HULL_MODEL:
            collider_id.args["generation_mode"] = "HULL_MODEL";
            break;
        case GenerationMode::HULL_MESHES:
            collider_id.args["generation_mode"] = "HULL_MESHES";
            break;
        case GenerationMode::DECOMPOSITION_MODEL:
            collider_id.args["generation_mode"] = "DECOMPOSITION_MODEL";
            break;
        case GenerationMode::DECOMPOSITION_MESHES:
            collider_id.args["generation_mode"] = "DECOMPOSITION_MESHES";
            break;
        }

        if (m_collider.id() == collider_id)
            return;

        m_collider = core::ResourceManager::get_collider(collider_id);
    }

    void ColliderComponent::render_wireframe(render::Shader const& shader) const
    {
        if (!m_collider)
            return;
        m_collider->render_wireframe(*entity, shader);
    }

    void ColliderComponent::serialize(serializer::Adapter& adapter)
    {
        adapter("generation_mode", *reinterpret_cast<int*>(&m_generation_mode));
    }

    BIRDY3D_REGISTER_DERIVED_TYPE_DEF(ecs::Component, ColliderComponent);

}
