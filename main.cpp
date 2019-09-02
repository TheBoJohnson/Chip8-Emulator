#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include <SDL2/SDL.h>
#include "chip8.h"

chip8 myChip8;

int main(int argc, char** argv) {
  if (argc != 2) {
	std::cerr << "Need 2 arguments to run" << std::endl;
	return 0;
  }

  // Set up renderer system and register input callbacks
  if (!myChip8.setupGraphics()) {
	std::cerr << "Error in setting up the graphics!" << std::endl;
	return 0;
  }

  // Get the name of the rom from the command line
  std::string romPath = std::string(argv[1]);
  
  // Initialize the Chip8 system and load the game into memory
  myChip8.initialize();
  //if (!myChip8.loadGame("roms/games/Soccer.ch8")) return 0;
  if (!myChip8.loadGame(romPath)) return 0;

  // Emulation Loop
  while (myChip8._chip8_window->_isOpen) {
	// Emulate one cycle
	myChip8.emulateCycle();

	// If the draw flag is set, update the screen
	if (myChip8._drawFlag) {
	  myChip8.drawGraphics();
	}

	// Store key press state (Press and Release)
	myChip8.setKeys();

	// Sleep to slow down emulation speed
	std::this_thread::sleep_for(std::chrono::microseconds(1200));
  }

  return 0;
}
