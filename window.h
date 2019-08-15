#ifndef __WINDOW_H_INCLUDED__
#define __WINDOW_H_INCLUDED__
#include <string>
#include <SDL2/SDL.h>

class Window {
public:
  Window(const std::string &title, int pixelSize, int width, int height);
  ~Window();

  //void pollEvents();
  SDL_Event* pollEvents();
  inline bool isClosed() const {return _closed;}
  inline SDL_Rect** getPixelArray() {return _pixelArray;}
  inline SDL_Renderer* getRenderer() const {return _renderer;} 

private:
  bool init();

  // Initial background of a black screen
  void setScreen() const;

  std::string _title;
  int _pixelSize;
  int _width;
  int _height;

  bool _closed = false;
  SDL_Rect* _pixelArray[64 * 32];

  SDL_Window* _window = nullptr;

protected:
  SDL_Renderer* _renderer = nullptr;
};

#endif
