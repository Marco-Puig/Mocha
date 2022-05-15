#pragma once

#include "m_camera.hpp"
#include "m_device.hpp"
#include "m_frame_info.hpp"
#include "m_game_object.hpp"
#include "m_pipeline.hpp"

// std
#include <memory>
#include <vector>

namespace m {
    class PointLightSystem {
    public:
        PointLightSystem(
            MDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~PointLightSystem();

        PointLightSystem(const PointLightSystem&) = delete;
        PointLightSystem& operator=(const PointLightSystem&) = delete;

        void update(FrameInfo& frameInfo, GlobalUbo& ubo);
        void render(FrameInfo& frameInfo);

    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

        MDevice& mDevice;

        std::unique_ptr<MPipeline> mPipeline;
        VkPipelineLayout pipelineLayout;
    };
}