#include <iostream>
#include <random>
#include <stdio.h>
#include "chip8.h"
#include "window.h"

const static int HEIGHT = 1600;
const static int WIDTH = 800;

bool chip8::setupGraphics() {
  // Create the window object
  _chip8_window = new window(HEIGHT, WIDTH);

  return _chip8_window->_isOpen;
}


void chip8::initialize() {
  // Initalize registers and memory once
  pc = 0x200; // Program counter starts at 0x200
  opcode = 0; // Reset current opcode
  I = 0; // Reset index register
  sp = 0; // Reset the stack pointer

  // Clear Display
  for (int i = 0; i < 64 * 32; ++i) {
	gfx[i] = 0;
  }

  //drawGraphics();
  _drawFlag = true;


  // Clear Stack and Registers
  for (int i = 0; i < 16; ++i) {
	stack[i] = 0;
	V[i] = 0;
  }

  // Clear Memory
  for (int i = 0; i < 4096; ++i) {
	memory[i] = 0;
  }

  // Load Fontset
  for (int i = 0; i < 80; ++i) {
	memory[i] = chip8_fontset[i];
  }

  // Reset Timers
  delay_timer = 0;
  sound_timer = 0;
}

// TODO: See if I implemented this code correctly. Lifted it from the fread page from the c++ reference
bool chip8::loadGame(std::string path) {
  FILE* romFile = nullptr;
  int lSize;
  char* buffer;
  size_t result;

  romFile = fopen(path.c_str(), "rb");
  if (romFile == nullptr) {
	std::cerr << "Could not open the romFile" << std::endl;
	return false;
  }
						   

  // obtain file sizes
  fseek(romFile, 0, SEEK_END);
  lSize = ftell(romFile);
  rewind(romFile);

  // allocate memory to contain the whole file
  buffer = (char*) malloc(sizeof(char) * lSize);
  if (buffer == nullptr) {
	std::cerr << "Buffer memory could not be allocated" << std::endl;
	return false;
  }

  // copy the file into the buffer
  result = fread(buffer, 1, lSize, romFile);

  // copy the contents from the buffer into the chip8's memory
  for (int i = 0; i < lSize; i++) {
	memory[512 + i] = buffer[i];
  }

  // terminate
  fclose(romFile);
  free(buffer);

  return true;
}

void chip8::emulateCycle() {
  // Fetch Opcode
  opcode = (memory[pc] << 8) | memory[pc + 1];
  //printf("Running opcode 0x%X\n", opcode);

  // Decode & Execute Opcode
  switch (opcode & 0xF000) {
  case 0x0000:
	switch (opcode & 0x000F) {
	case 0x0000:
	  // 0x00E0: Clears the Screen
	  for (int i = 0; i < 64 * 32; ++i) {
		gfx[i] = 0;
	  }

	  _drawFlag = true;

	  pc += 2;

	  break;

	case 0x000E:
	  // 0x00EE: Returns from subroutine
	  // TODO: Check to make sure this was implemented properly

	  // Set the program counter to the memory location that is held in the previous value of the sp
	  pc = stack[--sp];

	  pc += 2;

	  break;

	default:
	  printf("Unknown opcode: 0x%X\n", opcode);
	}

	break;

  case 0x1000:
	// 1NNN: Jumps to Address NNN
	pc = opcode & 0xFFF; 

	break;

  case 0x2000:
	// 2NNN: Calls subroutine at NNN

	// Save current instruction in the stack
	stack[sp++] = pc;

	// set the program counter to NNN
	pc = opcode & 0x0FFF;

	break;

  case 0x3000:
	// 3XNN: Skips the next instruction of VX is equal to NN (Usually the next instruction is a jump to skip a code block)
	if (V[(opcode >> 8) & 0xF] == (opcode & 0xFF)) {
	  pc += 4;
	} else {
	  pc += 2;
	}

	break;

  case 0x4000:
	// 4XNN: Skips the next instruction if VX doesn't equal NN. (Usually the next instruction is a jump to skip a code block) 
	if (V[(opcode >> 8) & 0xF] != (opcode & 0xFF)) {
	  pc += 4;
	} else {
	  pc += 2;
	}

	break;

  case 0x5000:
	// 5XY0: Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block) 
	if (V[(opcode >> 8) & 0xF] == V[(opcode >> 4) & 0xF]) {
	  pc += 4;
	} else {
	  pc += 2;
	}

	break;

  case 0x6000:
	// 6XNN: Sets VX to NN
	V[(opcode >> 8) & 0xF] = opcode & 0xFF;
	pc += 2;

	break;

  case 0x7000:
	// 7XNN: Adds NN to VX. (Carry flag is not changed)
	V[(opcode >> 8) & 0xF] += opcode & 0xFF;
	pc +=2;

	break;

  case 0x8000:
	switch (opcode & 0xF) {
	case 0x0000:
	  // 8XY0: Sets VX to the value of VY
	  V[(opcode >> 8) & 0xF] = V[(opcode >> 4) & 0xF];
	  pc +=2 ;
	  
	  break;

	case 0x0001:
	  // 8XY1: Sets VX to VX or VY. (Bitwise OR operation) 
	  V[(opcode >> 8) & 0xF] |= V[(opcode >> 4) & 0xF];
	  pc +=2 ;
	  
	  break;

	case 0x0002:
	  // 8XY2: Sets VX to VX and VY. (Bitwise AND operation) 
	  V[(opcode >> 8) & 0xF] &= V[(opcode >> 4) & 0xF];
	  pc +=2 ;

	  break;

	case 0x0003:
	  // 8XY3: Sets VX to VX xor VY.
	  V[(opcode >> 8) & 0xF] ^= V[(opcode >> 4) & 0xF];
	  pc +=2 ;

	  break;

	case 0x0004:
	  // 8XY4: Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't. 
	  if (V[(opcode >> 8) & 0xF] + V[(opcode >> 4) & 0xF] > 255) {
		V[0xF] = 1;
	  } else {
		V[0xF] = 0;
	  }

	  V[(opcode >> 8) & 0xF] += V[(opcode >> 4) & 0xF];

	  pc += 2;

	  break;

	case 0x0005:
	  // 8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't. 
	  if (V[(opcode >> 8) & 0xF] - V[(opcode >> 4) & 0xF] < 0) {
		V[0xF] = 1;
	  } else {
		V[0xF] = 0;
	  }

	  V[(opcode >> 8) & 0xF] -= V[(opcode >> 4) & 0xF];

	  pc += 2;

	  break;

	case 0x0006:
	  // 8XY6: Stores the least significant bit of VX in VF and then shifts VX to the right by 1.
	  V[0xF] = V[(opcode >> 8) & 0xF] & 1;
	  V[(opcode >> 8) & 0xF] >>= 1;

	  pc += 2;

	  break;

	case 0x0007:
	  // 8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't. 
	  if (V[(opcode >> 4) & 0xF] - V[(opcode >> 8) & 0xF] < 0) {
		V[0xF] = 1;
	  } else {
		V[0xF] = 0;
	  }

	  V[(opcode >> 8) & 0xF] = V[(opcode >> 4) & 0xF] - V[(opcode >> 8) & 0xF];

	  pc += 2;

	  break;

	case 0x000E:
	  // 8XYE: Stores the most significant bit of VX in VF and then shifts VX to the left by 1.
	  V[0xF] = (V[(opcode >> 8) & 0xF]) >> 7;

	  V[(opcode >> 8) & 0xF] <<= 1;

	  pc += 2;

	  break;

	default:
	  printf("Unknown opcode: 0x%X\n", opcode);
	}
	break;

  case 0x9000:
	// 9XY0: Skips the next instruction if VX doesn't equal VY. (Usually the next instruction is a jump to skip a code block) 
	if (V[(opcode >> 8) & 0xF] != V[(opcode >> 4) & 0xF]) {
	  pc += 4;
		} else {
	  pc += 2;
	}

	break;

  case 0xA000:
	// ANNN: Sets I to the address NNN
	I = opcode & 0x0FFF;
	pc += 2;

	break;

  case 0xB000:
	// BNNN: Jumps to the address NNN plus V0
	pc = V[0x0] + (opcode & 0x0FFF);

	break;

  case 0xC000:
	{
	// CXNN: Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN. 
	// TODO: Research to see if there is a cleaner / more efficient way to implement this opcode
	std::random_device rd;
	V[(opcode & 0x0F00) >> 8] = (rd() % 256) & (opcode & 0x00FF);

	pc += 2;

	}

	break;

  case 0xD000:
	{
	  // DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; I value doesn’t change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that doesn’t happen 
	  unsigned short x = V[(opcode & 0x0F00) >> 8];
	  unsigned short y = V[(opcode & 0x00F0) >> 4];
	  unsigned short height = opcode & 0x000F;
	  unsigned short pixel;

	  V[0xF] = 0;

	  for (int yline = 0; yline < height; ++yline) {
		pixel = memory[I + yline];

		for (int xline = 0; xline < 8; ++xline) {
		  if ((pixel & (0x80 >> xline)) != 0) {
			if (gfx[x + xline + ((y + yline) * 64)] == 1)
			  V[0xF] = 1;
			gfx[x + xline + ((y + yline) * 64)] ^= 1;
		  }
		}
	  }

	  _drawFlag = V[0xF];
	  pc += 2;

	}

	break;

  case 0xE000:
	switch (opcode & 0x000F) {
	case 0x000E:
	  // EX9E: Skips the next instruction if the key stored in VX is pressed. (Usually the next instruction is a jump to skip a code block) 
	  if (key[V[(opcode & 0x0F00) >> 8]] != 0) {
		pc += 4;
	  } else {
		pc += 2;
	  }

	  break;

	case 0x0001:
	  // EXA1: Skips the next instruction if the key stored in VX isn't pressed. (Usually the next instruction is a jump to skip a code block) 
	  if (key[V[(opcode & 0x0F00) >> 8]] == 0) {
		pc += 4;
	  } else {
		pc += 2;
	  }

	  break;

	default:
	  printf("Unknown opcode: 0x%X\n", opcode);
	  
	}

	break;

  case 0xF000:
	switch (opcode & 0x000F) {
	case 0x0007:
	  // FX07: Sets VX to the value of the delay timer
	  V[(opcode & 0x0F00) >> 8] = delay_timer;
												 
	  pc += 2;

	  break;

	case 0x000A:
	  {
	  // FX0A: A key press is awaited, then stored in X. (Blocking operaation. All instruction halted until next key event)

		printf("Running opocde: 0x%X\n", opcode);

		bool keyPressed = false;
		while (!keyPressed) {
		  setKeys();
		  for (int i = 0; i < 16; ++i) {
			if (key[i] != 0) {
			  V[(opcode & 0x0F00) >> 8] = i;
			  keyPressed = true;
			}
		  }
		}

		pc += 2;

		break;
	  }

	case 0x0005:
	  switch (opcode & 0x00F0) {
	  case 0x0010:
		// FX15: Sets the delay timer to VX
		delay_timer = V[(opcode & 0x0F00) >> 8];

		pc += 2;

		break;

	  case 0x0050:
		// FX55: Stores V0 to VX (including VX) in memory starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified. 
		// TODO: See if adding the extra parts from the original interpreter for this instruction helps with performance
		for (int offset = 0; offset <= ((opcode & 0x0F00) >> 8); ++offset) {
		  memory[I + offset] = V[offset];
		}

		pc += 2;

		break;

	  case 0x0060:
		// FX65: Fills V0 to VX (including VX) with values from memory starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified. 
		// TODO: See if adding the extra parts from the original interpreter for this instruction helps with performance
		for (int offset = 0; offset <= ((opcode & 0x0F00) >> 8); ++offset) {
		  V[offset] = memory[I + offset];
		}

		pc += 2;

		break;

	  default:
		printf("Unknown opcode: 0x%X\n", opcode);
		
	  }

	  break;

	case 0x0008:
	  // FX18: Sets the sound timer to VX
	  sound_timer = V[((opcode & 0x0F00) >> 8)];

	  pc += 2;

	  break;

	case 0x000E:
	  // FX1E: Adds VX to I
	  // TODO: Check to see if changing this opcode to the way it is implemented in the test emulator will improve performance
	  if (I + V[(opcode & 0x0F00) >> 8] > 0xFFF) {
		V[0xF] = 1;
	  } else {
		V[0xF] = 0;
	  }

	  I += V[((opcode & 0x0F00) >> 8)];

	  pc += 2;

	  break;

	case 0x0009:
	  // FX29: Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font
	  I = V[(opcode & 0x0F00) >> 8] * 5;

	  pc += 2;

	  break;

	case 0x0003:
	  // FX33: Stores the binary-coded decimal representation of VX, with the most significant of three digits at the address in I, the middle digit at I plus 1, and the least significant digit at I plus 2. (In other words, take the decimal representation of VX, place the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.) 
	  memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
	  memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
	  memory[I + 2] = (V[(opcode & 0x0F00) >> 8]) % 10;

	  pc += 2;

	  break;

	default:
	  printf("Unknown opcode: 0x%X\n", opcode);
	  
	}

	break;

  default:
	printf("Unknown opcode: 0x%X\n", opcode);
  }

  // Update Timers
  if (delay_timer > 0) {
	delay_timer--;
  }

  if (sound_timer == 1) {
	std::printf("BEEP!\n");
	sound_timer--;
  }

}

void chip8::drawGraphics() {
  SDL_Rect** pixelArray = _chip8_window->_pixelArray;

  for (int i = 0; i < (64 * 32); ++i) {
	if (gfx[i] == 1) {
	  SDL_SetRenderDrawColor(_chip8_window->_renderer, 255, 255, 255, 255);
	} else {
	  SDL_SetRenderDrawColor(_chip8_window->_renderer, 0, 0, 0, 255);
	}
	SDL_RenderFillRect(_chip8_window->_renderer, pixelArray[i]);
  }

  SDL_RenderPresent(_chip8_window->_renderer);
}

void chip8::setKeys() {
  // SDL_Event theEvent = _chip8_window->_eventHolder;

  // Test running SDL_Poll Event inside the setkeys function
  SDL_PollEvent(&_chip8_window->_eventHolder);

  switch (_chip8_window->_eventHolder.type) {
  case SDL_QUIT:
	std::cout << "Emulator closed" << std::endl;
	_chip8_window->_isOpen = false;
	break;

  case SDL_KEYDOWN:
	switch (_chip8_window->_eventHolder.key.keysym.sym) {
	case SDLK_1:
	  key[0x1] = 1;

	  for (int i = 1; i < 16; ++i)
		key[i] = 0;

	  std::cout << "Pressed 1" << std::endl;
	  break;

	case SDLK_2:
	  for (int i = 0; i < 16; ++i)
		key[i] = 0;

	  key[0x2] = 1;
	  break;
	
	case SDLK_3:
	  for (int i = 0; i < 16; ++i)
		key[i] = 0;

	  key[0x3] = 1;
	  break;

	case SDLK_4:
	  for (int i = 0; i < 16; ++i)
		key[i] = 0;

	  key[0xC] = 1;
	  break;

	case SDLK_q:
	  for (int i = 0; i < 16; ++i)
		key[i] = 0;

	  key[0x4] = 1;
	  break;

	case SDLK_w:
	  for (int i = 0; i < 16; ++i)
		key[i] = 0;

	  key[0x5] = 1;
	  break;

	case SDLK_e:
	  for (int i = 0; i < 16; ++i)
		key[i] = 0;

	  key[0x6] = 1;
	  break;
	
	case SDLK_r:
	  for (int i = 0; i < 16; ++i)
		key[i] = 0;

	  key[0xD] = 1;
	  break;

	case SDLK_a:
	  for (int i = 0; i < 16; ++i)
		key[i] = 0;

	  key[0x7] = 1;
	  break;

	case SDLK_s:
	  for (int i = 0; i < 16; ++i)
		key[i] = 0;

	  key[0x8] = 1;
	  break;

	case SDLK_d:
	  for (int i = 0; i < 16; ++i)
		key[i] = 0;

	  key[0x9] = 1;
	  break;

	case SDLK_f:
	  for (int i = 0; i < 16; ++i)
		key[i] = 0;

	  key[0xE] = 1;
	  break;

	case SDLK_z:
	  for (int i = 0; i < 16; ++i)
		key[i] = 0;

	  key[0xA] = 1;
	  break;

	case SDLK_x:
	  for (int i = 0; i < 16; ++i)
		key[i] = 0;

	  key[0x0] = 1;
	  break;

	case SDLK_c:
	  for (int i = 0; i < 16; ++i)
		key[i] = 0;

	  key[0xB] = 1;
	  break;

	case SDLK_v:
	  for (int i = 0; i < 16; ++i)
		key[i] = 0;

	  key[0xF] = 1;
	  break;
	}

	break;
  } 
}
  
