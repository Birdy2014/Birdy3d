#pragma once

#include "core/Base.hpp"
#include "ecs/Component.hpp"

namespace Birdy3d {

    class Shader;

    class Light : public Component {
    public:
        bool shadow_enabled;

        Light(bool shadow_enabled);
        virtual void use(const Shader& lightShader, int id, int textureid) = 0;
        virtual void setup_shadow_map() = 0;
        virtual void gen_shadow_map() = 0;
        void start() override;
        void update() override;
        void cleanup() override;

        template <class Archive>
        void serialize(Archive& ar) {
            ar(CEREAL_NVP(shadow_enabled));
        }

    protected:
        std::shared_ptr<Shader> m_depthShader;
        unsigned int m_depthMapFBO, m_depthMap;
        const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
        bool m_shadowMapUpdated = false;
    };

}

CEREAL_REGISTER_TYPE(Birdy3d::Light);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Birdy3d::Component, Birdy3d::Light);
