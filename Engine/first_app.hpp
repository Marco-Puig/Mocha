#pragma once

#include "m_descriptors.hpp"
#include "m_device.hpp"
#include "m_game_object.hpp"
#include "m_renderer.hpp"
#include "m_window.hpp"

// std
#include <memory>
#include <vector>

namespace m {
	class FirstApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		FirstApp();
		~FirstApp();

		FirstApp(const FirstApp&) = delete;
		FirstApp& operator=(const FirstApp&) = delete;

		void run();

	private:
		void loadGameObjects();

		MWindow mWindow{ WIDTH, HEIGHT, "Mocha Engine" };
		MDevice mDevice{ mWindow };
		MRenderer mRenderer{ mWindow, mDevice };

		// note: order of declarations matters
		std::unique_ptr<MDescriptorPool> globalPool{};
		MGameObject::Map gameObjects;
	};
}