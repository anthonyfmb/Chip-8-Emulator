#pragma once

#include <cstdint>
#include <random>

#define MEM_SIZE 4096
#define REGISTER_COUNT 16
#define STACK_SIZE 16
#define KEY_SIZE 16
#define GFX_HEIGHT 32
#define GFX_WIDTH 64
#define FONTSET_SIZE 80

// Because 0x000-0x1FF was reserved for the CHIP-8 interpreter, we will
// store instruction from the ROM and everything else starting at 0x200
#define START_ADDR 0x200
// We will need to access the fontset in the CHIP-8 interpreter
#define FONTSET_START_ADDR 0x50

class Chip8Emulator {
public:
  Chip8Emulator();
  void LoadROM(char const* filename);
  void Cycle();

  uint8_t keypad[KEY_SIZE];
  uint32_t video[GFX_HEIGHT * GFX_WIDTH];

private:
  // Debug unknown opcodes
  void Unknown(uint16_t);
  // Do nothing
	void OP_NULL();
	// CLS
	void OP_00E0();
	// RET
	void OP_00EE();
	// JP address
	void OP_1nnn();
	// CALL address
	void OP_2nnn();
	// SE Vx, byte
	void OP_3xkk();
	// SNE Vx, byte
	void OP_4xkk();
	// SE Vx, Vy
	void OP_5xy0();
	// LD Vx, byte
	void OP_6xkk();
	// ADD Vx, byte
	void OP_7xkk();
	// LD Vx, Vy
	void OP_8xy0();
	// OR Vx, Vy
	void OP_8xy1();
	// AND Vx, Vy
	void OP_8xy2();
	// XOR Vx, Vy
	void OP_8xy3();
	// ADD Vx, Vy
	void OP_8xy4();
	// SUB Vx, Vy
	void OP_8xy5();
	// SHR Vx
	void OP_8xy6();
	// SUBN Vx, Vy
	void OP_8xy7();
	// SHL Vx
	void OP_8xyE();
	// SNE Vx, Vy
	void OP_9xy0();
	// LD I, address
	void OP_Annn();
	// JP V0, address
	void OP_Bnnn();
	// RND Vx, byte
	void OP_Cxkk();
	// DRW Vx, Vy, height
	void OP_Dxyn();
	// SKP Vx
	void OP_Ex9E();
	// SKNP Vx
	void OP_ExA1();
	// LD Vx, DT
	void OP_Fx07();
	// LD Vx, K
	void OP_Fx0A();
	// LD DT, Vx
	void OP_Fx15();
	// LD ST, Vx
	void OP_Fx18();
	// ADD I, Vx
	void OP_Fx1E();
	// LD F, Vx
	void OP_Fx29();
	// LD B, Vx
	void OP_Fx33();
	// LD [I], Vx
	void OP_Fx55();
	// LD Vx, [I]
	void OP_Fx65();

  std::default_random_engine randGen;
  std::uniform_int_distribution<uint8_t> randByte;

  uint8_t memory[MEM_SIZE];
  uint8_t registers[REGISTER_COUNT];
  uint16_t index_register;
  uint16_t program_counter;
  uint16_t stack[STACK_SIZE];
  uint8_t stack_pointer;
  uint8_t delay_timer;
  uint8_t sound_timer;
  uint16_t opcode;
};

