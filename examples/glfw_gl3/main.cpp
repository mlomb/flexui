#include <iostream>
#include <cassert>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define GLFW_INCLUDE_ES3
#else
#define GLFW_INCLUDE_NONE
#include <gl/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <flexui/Element.hpp>

GLFWwindow* window = nullptr;
static bool main_loop_running = false;

void main_loop() {
	glClearColor(1.0, 0.3, 0.3, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	
	{
		const float verts[] = {
			1.0,  1.0,
		   -1.0,  1.0,
		   -1.0, -1.0,
		   -1.0, -1.0,
			1.0, -1.0,
			1.0,  1.0
		};
		unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, 12, verts, GL_STATIC_DRAW);

		using namespace flexui;
		Element* e = new Element();
		delete e;

		glDeleteBuffers(1, &VBO);
	}

	glfwSwapBuffers(window);
	glfwPollEvents();
}
static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

int main(int, char**) {
	glfwSetErrorCallback(error_callback);

	if (glfwInit() != GLFW_TRUE) {
		throw new std::runtime_error("GLFW failed to initialize");
	}

	window = glfwCreateWindow(1280, 720, "flexui demo", NULL, NULL);
	if (!window) {
		throw new std::runtime_error("GLFW window creation failed");
	}

	glfwMakeContextCurrent(window);

	#ifndef __EMSCRIPTEN__
	if (glewInit() != GLEW_OK) {
		throw new std::runtime_error("GLEW failed to initialize");
	}
	#endif

	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	std::cout << "Renderer: " << renderer << std::endl;
	std::cout << "OpenGL version supported " << version << std::endl;

	main_loop_running = true;

	#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop, 0, 1);
	#else
	while (main_loop_running) main_loop();
	#endif

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}