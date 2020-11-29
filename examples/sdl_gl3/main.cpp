#include <iostream>
#include <cassert>

#include <SDL.h>
#include <SDL_opengl.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <flexui/Element.hpp>

SDL_Window* window = nullptr;
SDL_GLContext context = nullptr;
static bool main_loop_running = false;

unsigned int width = 320 * 4;
unsigned int height = 180 * 4;

void main_loop() {
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT: main_loop_running = false; break;
		case SDL_KEYUP:
			if (event.key.keysym.sym == SDLK_ESCAPE)
				main_loop_running = false;
			break;
		}
	}


	glClearColor(1.0, 0.3, 0.3, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, width, height);
	
	{
		using namespace flexui;
		Element* e = new Element();
		delete e;
	}

	SDL_GL_SwapWindow(window);
}

int main(int, char**) {
	assert(SDL_Init(SDL_INIT_VIDEO) == 0);

	window = SDL_CreateWindow("flexui demo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (!window) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindow(): %s\n", SDL_GetError());
		return 1;
	}

	context = SDL_GL_CreateContext(window);
	if (!context) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_GL_CreateContext(): %s\n", SDL_GetError());
		return 1;
	}

	SDL_GL_SetSwapInterval(1);

	main_loop_running = true;

	#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop, 0, 1);
	#else
	while (main_loop_running) {
		main_loop();
		// SDL_Delay(16);
	}
	#endif

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}