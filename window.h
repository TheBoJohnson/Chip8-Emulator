#ifndef __WINDOW_HEADER__
#define __WINDOW_HEADER__

#include <SDL2/SDL.h>

class window {
 public:
  window(int height, int width);
  ~window();
  bool getWindowOpen() const;
  void setBackgroundColor(int r, int g, int b, int a);
  void pollEvent();

  int _height;
  int _width;
  bool _isOpen = true;
  SDL_Event _eventHolder;
  
  SDL_Window* _window;
  SDL_Renderer* _renderer;

  SDL_Rect* _pixelArray[64 * 32];


  bool init();

};

#endif
