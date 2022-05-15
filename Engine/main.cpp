#include "first_app.hpp"
#include <cstdlib>
#include <stdexcept>
#include <iostream>

int main() {
	m::FirstApp app{};

	try{
		app.run();
	} catch (const std::exception &e){
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;

}