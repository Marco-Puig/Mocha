#include "m_window.hpp"

// std
#include <stdexcept>

namespace m {

	MWindow::MWindow(int w, int h, std::string name) : width{ w }, height{ h }, windowName{ name }{
		initWindow();
	}

	MWindow::~MWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}
	
	void MWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}

	void MWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create window surface!");
		}
	}

	void MWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto mWindow = reinterpret_cast<MWindow*>(glfwGetWindowUserPointer(window));
		mWindow->framebufferResized = true;
		mWindow->width = width;
		mWindow->height = height;
	}

}

