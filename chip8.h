#ifndef __CHIP8_H_INCLUDED__
#define __CHIP8_H_INCLUDED__

#include "window.h"

class chip8 {
public:
  // Variable that holds the next opcode
  unsigned short opcode;

  // Array that holds system RAM
  unsigned char memory[4096];

  // System Registers
  unsigned char V[16];

  // Index Register
  unsigned short I;

  // Program Counter
  unsigned short pc;

  // Graphics Array
  unsigned char gfx[64 * 32];

  // Timers
  unsigned char delay_timer;
  unsigned char sound_timer;

  // System Stack and Stack Pointer
  unsigned short stack[16];
  unsigned short sp;

  // HEX basesd keypad
  unsigned char key[16];

  // Chip 8 font set
  unsigned char chip8_fontset[80] =
	{
	 0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	 0x20, 0x60, 0x20, 0x20, 0x70, // 1
	 0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	 0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	 0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	 0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	 0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	 0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	 0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	 0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	 0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	 0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	 0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	 0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	 0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	 0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

  bool drawFlag;


  bool setupGraphics();
  // void setupInput();
  void initialize();
  bool loadGame(std::string path);
  void emulateCycle();
  void drawGraphics();
  void setKeys();

  // TODO: Check to see if it's a good idea to set the Window as a pointer
  Window* window = nullptr;
};
#endif