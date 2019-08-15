#include <iostream>
#include <SDL2/SDL.h>
#include "window.h"

Window::Window(const std::string& title, int pixelSize, int width, int height) : _title(title), _pixelSize(pixelSize), _width(width), _height(height) {
  _closed = !init();
}

Window::~Window() {
  SDL_DestroyWindow(_window);
  SDL_DestroyRenderer(_renderer);
  SDL_Quit();
}

bool Window::init() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
	std::cerr << "Failed to init SDL" << std::endl;
	return false; 
  }

  _window = SDL_CreateWindow(_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _width, _height, 0);

  if (_window == nullptr) {
	std::cerr << "Failed to create window" << std::endl;
	return false;
  }

  _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_PRESENTVSYNC);

  if (_renderer == nullptr) {
	std::cerr << "Failed to create renderer" << std::endl;
	return false;
  }

  setScreen();

  // Create the array of Rects
  for (int i = 0; i < (64 * 32); i++) {
	_pixelArray[i] = new SDL_Rect();
	_pixelArray[i]->h = _pixelSize;
	_pixelArray[i]->w = _pixelSize;
	_pixelArray[i]->x = (i % 64) * _pixelSize;
	_pixelArray[i]->y = (i / 64) * _pixelSize;

  }


  return true;
}

/*
void Window::pollEvents() {
  SDL_Event event;

  if (SDL_PollEvent(&event)) {
	switch (event.type) {
	case SDL_QUIT:
	  _closed = true;
	  break;

	case SDL_KEYDOWN:
	  switch (event.key.keysym.sym) {
	  case SDLK_ESCAPE:
		_closed = true;
		break;
	  }
	  break;

	}
  }
}
*/

SDL_Event* Window::pollEvents() {
  SDL_Event* event = new SDL_Event();

  if (SDL_PollEvent(event)) {
	switch (event->type) {
	case SDL_QUIT:
	  _closed = true;
	  break;

	case SDL_KEYDOWN:
	  switch (event->key.keysym.sym) {
	  case SDLK_ESCAPE:
		_closed = true;
		break;

	  default:
		return event;
	  }
	  break;
	}
  }

  return nullptr;
}

void Window::setScreen() const {
  SDL_RenderPresent(_renderer); // actually sets the color on the window
  SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
  SDL_RenderClear(_renderer); // sets the color of the renderer
}
