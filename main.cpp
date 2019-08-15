#include <iostream>
#include "chip8.h"

chip8 myChip8;

int main(int argc, char** argv) {
  // Set up renderer system and register input callbacks
  if (!myChip8.setupGraphics()) {
	std::cerr << "Error in setting up the graphics!" << std::endl;
	return 0;
  }
  
  // TODO: See if you should delete setup input
  //myChip8.setupInput();

  // Initialize the Chip8 system and load the game into memory
  myChip8.initialize();
  if (!myChip8.loadGame("roms/games/Cave.ch8")) return 1;

  // Emulation Loop
  while (!myChip8.window->isClosed()) {
	// Emulate one cycle
	myChip8.emulateCycle();

	// If the draw flag is set, update the screen
	if (/*myChip8.drawFlag*/true) {
	  myChip8.drawGraphics();
	  std::cout << "Drew graphics" << std::endl;
	}

	// Store key press state (Press and Release)
	myChip8.setKeys();
  }

  return 0;
}
