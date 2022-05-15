#pragma once

#include "m_model.hpp"

//libs
#include <glm/gtc/matrix_transform.hpp>

// std
#include <memory>
#include <unordered_map>

namespace m {

    struct TransformComponent {
        glm::vec3 translation{};  // (position offset)
        glm::vec3 scale{ 1.f, 1.f, 1.f };
        glm::vec3 rotation;

        glm::mat4 mat4();

        glm::mat3 normalMatrix();
    };

    struct PointLightComponent {
        float lightIntensity = 1.0f;
    };

    class MGameObject {
    public:
        using id_t = unsigned int;
        using Map = std::unordered_map<id_t, MGameObject>;

        static MGameObject createGameObject() {
            static id_t currentId = 0;
            return MGameObject{ currentId++ };
        }

        static MGameObject makePointLight(
            float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f));

        MGameObject(const MGameObject&) = delete;
        MGameObject& operator=(const MGameObject&) = delete;
        MGameObject(MGameObject&&) = default;
        MGameObject& operator=(MGameObject&&) = default;

        id_t getId() { return id; }

        glm::vec3 color{};
        TransformComponent transform{};

        // Optional pointer components
        std::shared_ptr<MModel> model{};
        std::unique_ptr<PointLightComponent> pointLight = nullptr;

    private:
        MGameObject(id_t objId) : id{ objId } {}

        id_t id;
    };
}