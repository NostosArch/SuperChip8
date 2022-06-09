#pragma once

#include <cstdint>
#include<random>

const unsigned video_width_normal = 64;
const unsigned video_height_normal = 32;

const unsigned video_width_extended = 128;
const unsigned video_height_extended = 64;

const unsigned video_width = 128;
const unsigned video_height = 64;

bool extended_mode = false;

class Chip8
{

	public:
		void LoadROM(char const* filename);
		Chip8();
		uint8_t keypad[16]{};
		void Cycle();

		//Allocating space for Video with 128 width and 64 height size
		uint32_t video[video_width * video_height]{};

	private:
		//Function Pointer tables

		void Table0();
		void Table8();
		void TableE();
		void TableF();

		//Clear Display
		void OP_00E0();

		//Return opcode
		void OP_00EE();

		//Jump opcode
		void OP_1nnn();

		//Call opcode
		void OP_2nnn();

		//Skip next instruction if Vx == kk
		void OP_3xkk();

		//Skip next instruction if Vx != kk
		void OP_4xkk();

		//Skip next instruction if Vx == Vy
		void OP_5xy0();

		//Load byte kk into register Vx
		void OP_6xkk();

		//Add byte kk with register Vx and store it in register Vx
		void OP_7xkk();

		//Load contents of register Vy into register Vx
		void OP_8xy0();
		
		//OR the contents in registers Vx and Vy and store in Vx
		void OP_8xy1();

		//AND the contents in registers Vx and Vy and store in Vx
		void OP_8xy2();

		//XOR the contents in registers Vx and Vy and store in Vx
		void OP_8xy3();

		//Add the contents in registers Vx and Vy and store the sum in Vx and Carry in VF
		void OP_8xy4();

		//Subtract the contents in registers Vx and Vy and store the result in Vx and Not borrow in VF
		void OP_8xy5();

		//Shift register Vx by 1
		void OP_8xy6();

		//Subtract the contents in registers Vx and Vy and store the result in Vx 
		void OP_8xy7();

		//Check the most significant bit of Vx and set that value to VF and multiply Vx by 2
		void OP_8xyE();

		//Skip next instruction if register Vx and register Vy does not match
		void OP_9xy0();
	
		//Load the address into index
		void OP_Annn();

		//Jump to location registers[0] + nnn;
		void OP_Bnnn();

		//Set random byte & kk to registers[Vx]
		void OP_Cxkk();
		
		//Draw sprite on screen and If N=0 and extended mode, 16x16 
		void OP_Dxyn();
		
		//Skip next instruction if key (found in registers[Vx]) is pressed on keypad
		void OP_Ex9E();

		//Skip next instruction if key (found in registers[Vx]) is not pressed on keypad
		void OP_ExA1();

		//Load delay timer value in register Vx
		void OP_Fx07();

		//Wait for a key press and store the value of Key in Vx
		void OP_Fx0A();

		//Set Delay Timer with value in Vx
		void OP_Fx15();

		//Set sound timer with Value in Vx
		void OP_Fx18();

		//Add Vx to index register
		void OP_Fx1E();

		//It sets the index register to the starting address of Sprite digit in register Vx (basically Vx value)
		//Font Adddress start from 0x50 and every 5 bytes after that is a new digit
		void OP_Fx29();

		//Stores the hundreds position of value in Vx at I in memory, tens position at I+1 in memory, ones position at I+2 in memory
		void OP_Fx33();

		//Store registers from V0 till Vx starting from Index location in Memory
		void OP_Fx55();

		//Load registers from V0 till Vx starting from Index location in Memory
		void OP_Fx65();

		//Super Chip8 Emulator Instructions

		//Scroll Display N pixels down
		void OP_00Cn();

		//Scroll Display 4 pixels right
		void OP_00FB();

		//Scroll Display 4 pixels left
		void OP_00FC();

		//Exit CHIP Interpreter
		void OP_00FD();

		//Disable Extended Screen Mode
		void OP_00FE();

		//Enable Extended Screen Mode for full-screen graphics
		void OP_00FF();

		//Point I to 10-byte font sprite for digit VX(0..9)
		void OP_Fx30();

		//Store V0..VX in RPL user flags (X<=7)
		void OP_Fx75();

		//Read V0..VX from RPL user flags (X<=7)
		void OP_Fx85();

		//{} is used to initiailize the values with zeroes

		// 16 general purpose registers
		uint8_t registers[16]{};

		// 8 RPL user flags
		uint8_t rpl[8]{};

		//4KB memory space
		uint8_t memory[4096]{};

		//Index register used to store memory addresses
		uint16_t index{};

		//Program counter
		uint16_t pc{};

		//16 levels of stack for function calling
		uint16_t stack[16]{};

		//stack pointer
		uint8_t sp{};

		uint8_t delayTimer{};
		uint8_t soundTimer{};

		//Opcode used to determine the instruction type
		uint16_t opcode{};
		
		//For random initialization
		std::default_random_engine randGen;
		std::uniform_int_distribution<uint8_t> randByte;

		//Function pointer table for opcode decode and execution
		typedef void (Chip8::*Opcode) ();

		//Function Pointers given more space than required for easier mapping of opcodes
		Opcode table[0xFu + 1];
		Opcode table0[0xFFu + 1];
		Opcode tableE[0xEu + 1];
		Opcode table8[0xEu + 1];
		Opcode tableF[0x65u + 1];
};
