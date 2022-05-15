#include "first_app.hpp"

#include "m_imgui.hpp"
#include "keyboard_movement_controller.hpp"
#include "m_buffer.hpp"
#include "m_camera.hpp"
#include "point_light_system.hpp"
#include "simple_render_system.hpp"

// libs
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <chrono>
#include <stdexcept>

namespace m {


    FirstApp::FirstApp() {
        globalPool =
            MDescriptorPool::Builder(mDevice)
            .setMaxSets(MSwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MSwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();
        loadGameObjects();
    }

    FirstApp::~FirstApp() {}

    void FirstApp::run() {
        MImgui lveImgui{
            mWindow,
            mDevice,
            mRenderer.getSwapChainRenderPass(),
            mRenderer.getImageCount() };

        std::vector<std::unique_ptr<MBuffer>> uboBuffers(MSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < uboBuffers.size(); i++) {
            uboBuffers[i] = std::make_unique<MBuffer>(
                mDevice,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            uboBuffers[i]->map();
        }

        auto globalSetLayout =
            MDescriptorSetLayout::Builder(mDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .build();

        std::vector<VkDescriptorSet> globalDescriptorSets(MSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++) {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            MDescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
        }

        SimpleRenderSystem simpleRenderSystem{
            mDevice,
            mRenderer.getSwapChainRenderPass(),
            globalSetLayout->getDescriptorSetLayout() };
            PointLightSystem pointLightSystem{
               mDevice,
               mRenderer.getSwapChainRenderPass(),
               globalSetLayout->getDescriptorSetLayout() };
            MCamera camera{};

        auto viewerObject = MGameObject::createGameObject();
        viewerObject.transform.translation.z = -2.5f;
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();
        while (!mWindow.shouldClose()) {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime =
                std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.moveInPlaneXZ(mWindow.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = mRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

            if (auto commandBuffer = mRenderer.beginFrame()) {
                int frameIndex = mRenderer.getFrameIndex();
                FrameInfo frameInfo{
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    camera,
                    globalDescriptorSets[frameIndex],
                    gameObjects };

                // update
                GlobalUbo ubo{};
                ubo.projection = camera.getProjection();
                ubo.view = camera.getView();
                ubo.inverseView = camera.getInverseView();
                pointLightSystem.update(frameInfo, ubo);
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                // tell imgui that we're starting a new frame
                lveImgui.newFrame();
                
                // render
                mRenderer.beginSwapChainRenderPass(commandBuffer);

                // order here matters
                simpleRenderSystem.renderGameObjects(frameInfo);
                pointLightSystem.render(frameInfo);

                // example code telling imgui what windows to render, and their contents
                // this can be replaced with whatever code/classes you set up configuring your
                // desired engine UI
                lveImgui.runExample();

                // as last step in render pass, record the imgui draw commands
                lveImgui.render(commandBuffer);

                mRenderer.endSwapChainRenderPass(commandBuffer);
                mRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(mDevice.device());
    }

    void FirstApp::loadGameObjects() {
        std::shared_ptr<MModel> mModel =
            MModel::createModelFromFile(mDevice, "models/flat_vase.obj");
        auto flatVase = MGameObject::createGameObject();
        flatVase.model = mModel;
        flatVase.transform.translation = { -.5f, .5f, 0.f };
        flatVase.transform.scale = { 3.f, 1.5f, 3.f };
        gameObjects.emplace(flatVase.getId(), std::move(flatVase));

        mModel = MModel::createModelFromFile(mDevice, "models/smooth_vase.obj");
        auto smoothVase = MGameObject::createGameObject();
        smoothVase.model = mModel;
        smoothVase.transform.translation = { .5f, .5f, 0.f };
        smoothVase.transform.scale = { 3.f, 1.5f, 3.f };
        gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));

        mModel = MModel::createModelFromFile(mDevice, "models/quad.obj");
        auto floor = MGameObject::createGameObject();
        floor.model = mModel;
        floor.transform.translation = { 0.f, .5f, 0.f };
        floor.transform.scale = { 3.f, 1.f, 3.f };
        gameObjects.emplace(floor.getId(), std::move(floor));

        std::vector<glm::vec3> lightColors{
        {1.f, .1f, .1f},
        {.1f, .1f, 1.f},
        {.1f, 1.f, .1f},
        {1.f, 1.f, .1f},
        {.1f, 1.f, 1.f},
        {1.f, 1.f, 1.f}};

        for (int i = 0; i < lightColors.size(); i++) {
            auto pointLight = MGameObject::makePointLight(0.2f);
            pointLight.color = lightColors[i];
            auto rotateLight = glm::rotate(
                glm::mat4(1.f),
                (i * glm::two_pi<float>()) / lightColors.size(),
                { 0.f, -1.f, 0.f });
            pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
            gameObjects.emplace(pointLight.getId(), std::move(pointLight));
        }
    }

}