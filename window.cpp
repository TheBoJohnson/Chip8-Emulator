#include <iostream>
#include "window.h"

window::window(int height, int width) : _height(height), _width(width){
  _isOpen = init();
}

window::~window() {
  SDL_DestroyWindow(_window);
  SDL_DestroyRenderer(_renderer);
  SDL_Quit();
}

bool window::getWindowOpen() const {
  return _isOpen;
}

void window::setBackgroundColor(int r, int g, int b, int a) {
	SDL_SetRenderDrawColor(_renderer, r, g, b, a);
	SDL_RenderClear(_renderer);
	SDL_RenderPresent(_renderer);
}

void window::pollEvent() {
  if (_eventHolder.type == SDL_WINDOWEVENT && _eventHolder.window.event == SDL_WINDOWEVENT_CLOSE) {
	_isOpen = false;
  } else if (_eventHolder.type == SDL_KEYDOWN && _eventHolder.key.keysym.sym == SDLK_1) {

  }
}

bool window::init() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
	return false;
  }

  _window = SDL_CreateWindow("Chip8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _height, _width, 0);

  if (_window == nullptr) return false;

  _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
  //_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_PRESENTVSYNC);

  if (_renderer == nullptr) return false;

  // Initializinng the array of pixels
  // The array is 64 * 32 so each pixel will be a 25 x 25 rectangle
  for (int i = 0; i < 64 * 32; ++i) {
	_pixelArray[i] = new SDL_Rect();
	_pixelArray[i]->w = 25;
	_pixelArray[i]->h = 25;
	_pixelArray[i]->x = (i % 64) * 25;
	_pixelArray[i]->y = (i / 64) * 25;
  }


  // Set the background color
  for (int i = 0; i < 100; ++i) {
	this->setBackgroundColor(0, 0, 0, 255);
  }

  SDL_Delay(1000);


  return true;
}
