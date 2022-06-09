#pragma once
#include<cstdint>

class SDL_Window;
class SDL_Renderer;
class SDL_Texture;

class Platform {

		SDL_Window *window;
		SDL_Renderer *renderer;
		SDL_Texture *texture;

	public : 

		Platform(int windowWidth, int windowHeight, int textureWidth, int textureHeight);
		~Platform();
		void Update(void const* buffer, int pitch);
		bool ProcessInput(uint8_t* keys);

};
