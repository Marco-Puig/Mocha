#pragma once

#include "m_device.hpp"
#include "m_swap_chain.hpp"
#include "m_window.hpp"

// std
#include <cassert>
#include <memory>
#include <vector>

namespace m {
    class MRenderer {
    public:
        MRenderer(MWindow& window, MDevice& device);
        ~MRenderer();

        MRenderer(const MRenderer&) = delete;
        MRenderer& operator=(const MRenderer&) = delete;

        VkRenderPass getSwapChainRenderPass() const { return mSwapChain->getRenderPass(); }
        float getAspectRatio() const { return mSwapChain->extentAspectRatio(); }
        uint32_t getImageCount() const { return mSwapChain->imageCount(); }
        bool isFrameInProgress() const { return isFrameStarted; }

        VkCommandBuffer getCurrentCommandBuffer() const {
            assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
            return commandBuffers[currentFrameIndex];
        }

        int getFrameIndex() const {
            assert(isFrameStarted && "Cannot get frame index when frame not in progress");
            return currentFrameIndex;
        }

        VkCommandBuffer beginFrame();
        void endFrame();
        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

    private:
        void createCommandBuffers();
        void freeCommandBuffers();
        void recreateSwapChain();

        MWindow& mWindow;
        MDevice& mDevice;
        std::unique_ptr<MSwapChain> mSwapChain;
        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex;
        int currentFrameIndex = 0;
        bool isFrameStarted = false;
    };
}