#pragma once

#include "m_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace m {

    class MDescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(MDevice& mDevice) : mDevice{ mDevice } {}

            Builder& addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<MDescriptorSetLayout> build() const;

        private:
            MDevice& mDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        MDescriptorSetLayout(
            MDevice& mDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~MDescriptorSetLayout();
        MDescriptorSetLayout(const MDescriptorSetLayout&) = delete;
        MDescriptorSetLayout& operator=(const MDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        MDevice& mDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class MDescriptorWriter;
    };

    class MDescriptorPool {
    public:
        class Builder {
        public:
            Builder(MDevice& mDevice) : mDevice{ mDevice } {}

            Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& setMaxSets(uint32_t count);
            std::unique_ptr<MDescriptorPool> build() const;

        private:
            MDevice& mDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        MDescriptorPool(
            MDevice& mDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~MDescriptorPool();
        MDescriptorPool(const MDescriptorPool&) = delete;
        MDescriptorPool& operator=(const MDescriptorPool&) = delete;

        bool allocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void resetPool();

    private:
        MDevice& mDevice;
        VkDescriptorPool descriptorPool;

        friend class MDescriptorWriter;
    };

    class MDescriptorWriter {
    public:
        MDescriptorWriter(MDescriptorSetLayout& setLayout, MDescriptorPool& pool);

        MDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        MDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool build(VkDescriptorSet& set);
        void overwrite(VkDescriptorSet& set);

    private:
        MDescriptorSetLayout& setLayout;
        MDescriptorPool& pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

}