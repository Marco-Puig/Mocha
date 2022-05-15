#pragma once

#include "m_camera.hpp"
#include "m_device.hpp"
#include "m_frame_info.hpp"
#include "m_game_object.hpp"
#include "m_pipeline.hpp"

//std
#include <memory>
#include <vector>

namespace m {
	class SimpleRenderSystem {
	public:
		SimpleRenderSystem(
			MDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void renderGameObjects(FrameInfo& frameInfo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		MDevice& mDevice;

		std::unique_ptr<MPipeline> mPipeline;
		VkPipelineLayout pipelineLayout;
	};
}