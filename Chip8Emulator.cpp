#include <fstream>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <random>
#include <iostream>

#include "Chip8Emulator.hpp"

uint8_t fontset[FONTSET_SIZE] =
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

Chip8Emulator::Chip8Emulator() : randGen(std::chrono::system_clock::now().time_since_epoch().count()) {
  program_counter = START_ADDR;

  randByte = std::uniform_int_distribution<uint8_t>(0, 255U);
  // Load fonts into memory
  for (unsigned int i = 0; i < FONTSET_SIZE; i++) {
    memory[FONTSET_START_ADDR + i] = fontset[i];
  }

}

void Chip8Emulator::LoadROM(char const* filename) {
  // Opens ROM file as binary stream
  // Move file pointer to EOF to get size
  std::ifstream file(filename, std::ios::binary | std::ios::ate);

  if (file.is_open()) {
    // Then create buffer to hold contents
    std::streampos size = file.tellg();
    char* buffer = new char[size];

    // Go back to beginning and fill with buffer
    file.seekg(0, std::ios::beg);
    file.read(buffer, size); 
    file.close();

    // Load ROM into Chip8's memory
    for (long i = 0; i < size; i++) {
      memory[START_ADDR + i] = buffer[i];
    }

    // Free buffer
    delete[] buffer;
  }
}

void Chip8Emulator::Cycle() {
  opcode = (memory[program_counter] << 8u) | memory[program_counter + 1];

  program_counter += 2;

  // Start by getting the highest order byte and work downwards
  switch (opcode & 0x0F000u) {
    case 0x0000:
      switch (opcode & 0x00FFu) {
        case 0x00E0:
          OP_00E0();
          break;
        case 0x00EE:
          OP_00EE();
          break;
        default:
          Unknown(opcode);
          break;
       }
      break;
    case 0x1000:
      OP_1nnn();
      break;
    case 0x2000:
      OP_2nnn();
      break;
    case 0x3000:
      OP_3xkk();
      break;
    case 0x4000:
      OP_4xkk();
      break;
    case 0x5000:
      OP_5xy0();
      break;
    case 0x6000:
      OP_6xkk();
      break;
    case 0x7000:
      OP_7xkk();
      break;
    case 0x8000:
      switch (opcode & 0x000Fu) {
        case 0x0:
          OP_8xy0();
          break;
        case 0x1:
          OP_8xy1();
          break;
        case 0x2:
          OP_8xy2();
          break;
        case 0x3:
          OP_8xy3();
          break;
        case 0x4:
          OP_8xy4();
          break;
        case 0x5:
          OP_8xy5();
          break;
        case 0x6:
          OP_8xy6();
          break;
        case 0x7:
          OP_8xy7();
          break;
        case 0xE:
          OP_8xyE();
          break;
        default:
          Unknown(opcode);
          break;
      }
      break;
    case 0x9000:
      switch (opcode & 0x000Fu) {
        case 0x0:
          OP_9xy0();
          break;
        default:
          Unknown(opcode);
          break;
      }
      break;
    case 0xA000:
      OP_Annn();
      break;
    case 0xB000:
      OP_Bnnn();
      break;
    case 0xC000:
      OP_Cxkk();
      break;
    case 0xD000:
      OP_Dxyn();
      break;
    case 0xE000:
      switch (opcode & 0x00FFu) {
        case 0x9E:
          OP_Ex9E();
          break;
        case 0xA1:
          OP_ExA1();
          break;
        default:
          Unknown(opcode);
          break;
      }
      break;
    case 0xF000:
      switch (opcode & 0x00FFu) {
        case 0x07:
          OP_Fx07();
          break;
        case 0x0A:
          OP_Fx0A();
          break;
        case 0x15:
          OP_Fx15();
          break;
        case 0x18:
          OP_Fx18();
          break;
        case 0x1E:
          OP_Fx1E();
          break;
        case 0x29:
          OP_Fx29();
          break;
        case 0x33:
          OP_Fx33();
          break;
        case 0x55:
          OP_Fx55();
          break;
        case 0x65:
          OP_Fx65();
          break;
        default:
          Unknown(opcode);
      }  
    default:
      Unknown(opcode);
      break;
  }
}

void Chip8Emulator::Unknown(uint16_t op) {
  std::cout << "Unknown opcode: " <<  op << std::endl;
} 

/* The following are the 34 instructions we need to emulate
 * 
 * For reference:
 * (opcode & 0x0F00u) >> 8u gets the lower 4 bits of the high byte
 * (opcode & 0x00F0u) >> 4u gets the upper 4 bits of the low byte
 * opcode & 0x000Fu gets the lowest 4 bits
 * opcode & 0x00FFu gets the lowest 8 bits
 * opcode & 0x0FFFu gets the lowest 12 bits
 */

// CLS: Clear the display
void Chip8Emulator::OP_00E0() {
  memset(video, 0, sizeof(video));
}

// RET: Return from a subroutine
void Chip8Emulator::OP_00EE() {
  // We substact from the stack pointer to go back to where it was
  // when it hit a CALL instruction
  --stack_pointer;
  program_counter = stack[stack_pointer];
}

// JP addr: Jump to location nnn
void Chip8Emulator::OP_1nnn() {
  uint16_t addr = opcode & 0x0FFFu;
  
  program_counter = addr;
}

// CALL addr: Call subroutine at nnn
void Chip8Emulator::OP_2nnn() {
  uint16_t addr = opcode & 0x0FFFu;

  stack[stack_pointer] = program_counter;
  ++stack_pointer;
  program_counter = addr;
}

// SE Vx, Byte: if Vx = Byte, skip next instruction
void Chip8Emulator::OP_3xkk() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t byte = opcode & 0x00FFu;

  if (registers[Vx] == byte) {
    program_counter += 2; // Skips next instruction
  }
}

// SNE Vx, Byte: if Vx != Byte, skip next instruction
void Chip8Emulator::OP_4xkk() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t byte = opcode & 0x00FFu;
  
  if (registers[Vx] != byte) {
    program_counter += 2; // Skips next instruction
  }
}

// SE Vx, Vy: if Vx = Vy, skip next instruction
void Chip8Emulator::OP_5xy0() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;

  if (registers[Vx] == registers[Vy]) {
    program_counter += 2; // Skips next instruction
  }
}

// SNE Vx, Vy: if Vx != Vy, skip next instruction
void Chip8Emulator::OP_9xy0() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;

  if (registers[Vx] != registers[Vy]) {
    program_counter += 2;
  }
}

// LD Vx, Byte: set Vx = Byte
void Chip8Emulator::OP_6xkk() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t byte = opcode & 0x00FFu;

  registers[Vx] = byte;
}

// ADD Vx, byte: add byte to Vx
void Chip8Emulator::OP_7xkk() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t byte = opcode * 0x00FFu;
  
  registers[Vx] += byte;
}

// LD Vx, Vy: set Vx = Vy
void Chip8Emulator::OP_8xy0() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;

  registers[Vx] = registers[Vy];
}

// OR Vx, Vy: set Vx to Vx OR Vy
void Chip8Emulator::OP_8xy1() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;

  registers[Vx] |= registers[Vy];
}

// AND Vx, Vy: set Vx to Vx AND Vy
void Chip8Emulator::OP_8xy2() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;

  registers[Vx] &= registers[Vy];
}

// XOR Vx, Vy: set Vx to Vx XOR Vy
void Chip8Emulator::OP_8xy3() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;

  registers[Vx] ^= registers[Vy];
}

// ADD Vx, Vy: set Vx = Vx + Vy, and set VF = carry (overflow flag)
void Chip8Emulator::OP_8xy4() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;

  // We store the sum in 16 bits in case it is larger than 8 
  uint16_t sum = registers[Vx] + registers[Vy];

  // If sum is greater than 8 bits, set flag to 1
  if (sum > 255u) {
    registers[0xF] = 1;
  } 
  else {
    registers[0xF] = 0;
  }

  // Store only the first 8 bits of sum
  registers[Vx] = sum & 0xFFu;
}


void Chip8Emulator::OP_8xy5() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;

  if (registers[Vx] > registers[Vy])
  {
    registers[0xF] = 1;
  }
  else
  {
    registers[0xF] = 0;
  }

  registers[Vx] -= registers[Vy];
}

void Chip8Emulator::OP_8xy6() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;

  // Save LSB in VF
  registers[0xF] = (registers[Vx] & 0x1u);

  registers[Vx] >>= 1;
}

// SUBN Vx, Vy: set Vx = Vy - Vx, and set VF = NOT borrow
void Chip8Emulator::OP_8xy7() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;

  // If Vy is larger than Vx, then the result will not be negative
  if (registers[Vy] > registers[Vx]) {
    registers[0xF] = 1;
  }
  else {
    registers[0xF] = 0;
  }

  registers[Vx] = registers[Vy] - registers[Vx];
}

// SHL Vx {, Vy}: if the MSB of Vx is 1, then VF is set to 1, then Vx is 
// multiplied by 2
void Chip8Emulator::OP_8xyE() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;

  registers[0xF] = (registers[Vx] & 0x80u) >> 7u; // Set VF to MSB of Vx
  
  registers[Vx] <<= 1; // Left shift for multiplication by 2
}

// LD I, addr: set index = nnn
void Chip8Emulator::OP_Annn() {
  uint16_t addr = opcode & 0x0FFFu;

  index_register = addr;
}

// JP V0, addr: jump to location nnn + V0
void Chip8Emulator::OP_Bnnn() {
  uint16_t addr = opcode & 0x0FFFu;

  program_counter = registers[0] + addr;
}

// RND Vx, Byte: set Vx = a random byte AND byte
void Chip8Emulator::OP_Cxkk() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t byte = opcode & 0x00FFu;

  registers[Vx] = randByte(randGen) & byte;
}

// DRW Vx, Vy, nibble: draw sprite starting at (Vx, Vy), set VF to 1 if
// there is a collision with another sprite
void Chip8Emulator::OP_Dxyn() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;
  uint8_t height = opcode & 0x000Fu;

  // Wraps around if goes beyond screen boundaries
  uint8_t xPos = registers[Vx] % GFX_WIDTH;
  uint8_t yPos = registers[Vy] % GFX_HEIGHT;

  registers[0xF] = 0;

  for (unsigned int row = 0; row < height; row++) {
    uint8_t spriteByte = memory[index_register + row];

    for (unsigned int col = 0; col < 8; col++) {
      uint8_t spritePixel = spriteByte & (0x80u >> col);
      uint32_t *screenPixel = &video[(yPos + row) * GFX_WIDTH + (xPos + col)];

      // If both the sprite pixel and screen pixel are on, set VF to 1
      if (spritePixel) {
        if (*screenPixel == 0xFFFFFFFF) {
          registers[0xF] = 1;
        }

        // Using XOR we can take care of collisions
        *screenPixel ^= 0xFFFFFFFF;
      }
    }
  }
}

// SKP Vx: skip next instruction if key with value of Vx is pressed
void Chip8Emulator::OP_Ex9E() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t key = registers[Vx];

  if (keypad[key]) {
    program_counter += 2;
  }
}

// SKNP Vx: skip next instruction if key with value of Vx is not pressed
void Chip8Emulator::OP_ExA1() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t key = registers[Vx];

  if (!keypad[key]) {
    program_counter += 2;
  }
}

// LD Vx, DT: set Vx to delay timer value
void Chip8Emulator::OP_Fx07() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;

  registers[Vx] = delay_timer;
}

// LD Vx, K: wait for key press, then store value of key in Vx
void Chip8Emulator::OP_Fx0A() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;

  if (keypad[0]) {
    registers[Vx] = 0;
  }
  else if (keypad[1]) {
    registers[Vx] = 1;
  }
  else if (keypad[2]) {
    registers[Vx] = 2;
  }
  else if (keypad[3]) {
    registers[Vx] = 3;
  }
  else if (keypad[4]) {
    registers[Vx] = 4;
  }
  else if (keypad[5]) {
    registers[Vx] = 5;
  }
  else if (keypad[6]) {
    registers[Vx] = 6;
  }
  else if (keypad[7]) {
    registers[Vx] = 7;
  }
  else if (keypad[8]) {
    registers[Vx] = 8;
  }
  else if (keypad[9]) {
    registers[Vx] = 9;
  }
  else if (keypad[10]) {
    registers[Vx] = 10;
  }
  else if (keypad[11]) {
    registers[Vx] = 11;
  }
  else if (keypad[12]) {
    registers[Vx] = 12;
  }
  else if (keypad[13]) {
    registers[Vx] = 13;
  }
  else if (keypad[14]) {
    registers[Vx] = 14;
  }
  else if (keypad[15]) {
    registers[Vx] = 15;
  }
  else {
    program_counter -=2;
  }
}

// LD DT, Vx: set delay timer to Vx
void Chip8Emulator::OP_Fx15() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;

  delay_timer = registers[Vx];
}

// LD ST, Vx: set sound timer to Vx
void Chip8Emulator::OP_Fx18() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;

  sound_timer = registers[Vx];
}

// ADD I, Vx: add Vx to index
void Chip8Emulator::OP_Fx1E() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;

  index_register += registers[Vx];
}

// LD F, Vx: set index to location of sprite for digit Vx
void Chip8Emulator::OP_Fx29() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t digit = registers[Vx];

  index_register = FONTSET_START_ADDR + (5 * digit);
}

// LD B, Vx: store BCD representation of Vx in memory locations I, I+1,
// and I+2
void Chip8Emulator::OP_Fx33() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t val = registers[Vx];

  // Ones-place
  memory[index_register + 2] = val % 10;
  val /= 10;
  
  // Tens-place
  memory[index_register + 1] = val % 10;
  val /= 10;

  // Hundreds-place
  memory[index_register] = val % 10;
}

// LD [I], Vx: store registers V0 through Vx in memory starting at location I
void Chip8Emulator::OP_Fx55() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;

  for (uint8_t i = 0; i <= Vx; i++) {
    memory[index_register + i] = registers[i];
  }
}

// LD Vx, [I]: read registers V0 through Vx from memory starting at location I
void Chip8Emulator::OP_Fx65() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;

  for (uint8_t i = 0; i <= Vx; i++) {
    registers[i] = memory[index_register + i];
  }
}

