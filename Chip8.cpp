#include<iostream>
#include "Chip8.hpp"
#include <fstream>
#include<chrono>
#include<random>
#include<cstring>

const unsigned int start_address = 0x200;
const unsigned int font_start_address = 0x50;
const unsigned int extended_font_start_address = 0xA0;
const unsigned int fontset_size = 240;


//Loading ROM into the memory space
void Chip8::LoadROM(char const* filename){

	std::ifstream rom_file(filename, std::ios::binary | std::ios::ate);
	
	if(rom_file.is_open()){

		std::streampos size = rom_file.tellg();

		char* buffer = new char[size];

		rom_file.seekg(0, std::ios::beg);
		rom_file.read(buffer, size);
		rom_file.close();

		for(long i=0; i<size; i++){

			memory[start_address + i] = buffer[i];

		}

		delete[] buffer;
	}

}

//Initializing the other memory locations
Chip8::Chip8():randGen(std::chrono::system_clock::now().time_since_epoch().count()){

	pc = start_address;

	uint8_t fontset[fontset_size] = {

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
	0xF0, 0x80, 0xF0, 0x80, 0x80,  // F



	//High resolution mode fonts for digits 0..9	

        0x3C, 0x7E, 0xE7, 0xC3, 0xC3, 0xC3, 0xC3, 0xE7, 0x7E, 0x3C, //0 
        0x18, 0x38, 0x58, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, //1
        0x3E, 0x7F, 0xC3, 0x06, 0x0C, 0x18, 0x30, 0x60, 0xFF, 0xFF, //2
        0x3C, 0x7E, 0xC3, 0x03, 0x0E, 0x0E, 0x03, 0xC3, 0x7E, 0x3C, //3
        0x06, 0x0E, 0x1E, 0x36, 0x66, 0xC6, 0xFF, 0xFF, 0x06, 0x06, //4
        0xFF, 0xFF, 0xC0, 0xC0, 0xFC, 0xFE, 0x03, 0xC3, 0x7E, 0x3C, //5
        0x3E, 0x7C, 0xC0, 0xC0, 0xFC, 0xFE, 0xC3, 0xC3, 0x7E, 0x3C, //6
        0xFF, 0xFF, 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x60, 0x60, //7
        0x3C, 0x7E, 0xC3, 0xC3, 0x7E, 0x7E, 0xC3, 0xC3, 0x7E, 0x3C, //8
        0x3C, 0x7E, 0xC3, 0xC3, 0x7F, 0x3F, 0x03, 0x03, 0x3E, 0x7C  //9

	};

	for(int i = 0; i < fontset_size; i++){
	
		memory[font_start_address+i] = fontset[i];

	}

	randByte = std::uniform_int_distribution<uint8_t> (0,255U);
	
	table[0] = &Chip8::Table0;

	table0[0xC] = &Chip8::OP_00Cn;
	table0[0xE0] = &Chip8::OP_00E0;
	table0[0xEE] = &Chip8::OP_00EE;
	table0[0xFB] = &Chip8::OP_00FB;
	table0[0xFC] = &Chip8::OP_00FC;
	table0[0xFD] = &Chip8::OP_00FD;
	table0[0xFE] = &Chip8::OP_00FE;
	table0[0xFF] = &Chip8::OP_00FF;

	table[0x1] = &Chip8::OP_1nnn;  	
	table[0x2] = &Chip8::OP_2nnn;  	
	table[0x3] = &Chip8::OP_3xkk;  	
	table[0x4] = &Chip8::OP_4xkk;  	
	table[0x5] = &Chip8::OP_5xy0;  	
	table[0x6] = &Chip8::OP_6xkk;  	
	table[0x7] = &Chip8::OP_7xkk;

	table[0x8] = &Chip8::Table8;  	
	table8[0x0] = &Chip8::OP_8xy0;  	
	table8[0x1] = &Chip8::OP_8xy1;  	
	table8[0x2] = &Chip8::OP_8xy2;  	
	table8[0x3] = &Chip8::OP_8xy3;  	
	table8[0x4] = &Chip8::OP_8xy4;  	
	table8[0x5] = &Chip8::OP_8xy5;  	
	table8[0x6] = &Chip8::OP_8xy6;  	
	table8[0x7] = &Chip8::OP_8xy7;  	
	table8[0xE] = &Chip8::OP_8xyE;  	

	table[0x9] = &Chip8::OP_9xy0;  	
	table[0xA] = &Chip8::OP_Annn;  	
	table[0xB] = &Chip8::OP_Bnnn;  	
	table[0xC] = &Chip8::OP_Cxkk;  	
	table[0xD] = &Chip8::OP_Dxyn;  	

	table[0xE] = &Chip8::TableE;  	
	tableE[0x1] = &Chip8::OP_ExA1;  	
	tableE[0xE] = &Chip8::OP_Ex9E;

	table[0xF] = &Chip8::TableF;  	
	tableF[0x07] = &Chip8::OP_Fx07;  	
	tableF[0x0A] = &Chip8::OP_Fx0A;  	
	tableF[0x15] = &Chip8::OP_Fx15;  	
	tableF[0x18] = &Chip8::OP_Fx18;  	
	tableF[0x1E] = &Chip8::OP_Fx1E;  	
	tableF[0x29] = &Chip8::OP_Fx29;  	
	tableF[0x30] = &Chip8::OP_Fx30;  	
	tableF[0x33] = &Chip8::OP_Fx33;  	
	tableF[0x55] = &Chip8::OP_Fx55;  	
	tableF[0x65] = &Chip8::OP_Fx65;  	
	tableF[0x75] = &Chip8::OP_Fx75;  	
	tableF[0x85] = &Chip8::OP_Fx85;  	

}

void Chip8::Table0(){

	uint8_t k = opcode & 0x00F0u; 

	if((opcode & 0x00F0u) == 192) {
		(this->*table0[0xC]) ();
	} else {
		(this->*table0[(opcode & 0x00FFu)]) ();
	}
}

void Chip8::Table8(){
	(this->*table8[(opcode & 0x000Fu)]) ();
}

void Chip8::TableE(){
	(this->*tableE[(opcode & 0x000Fu)]) ();
}

void Chip8::TableF(){
	(this->*tableF[(opcode & 0x00FFu)]) ();
}

void Chip8::Cycle(){
	
	//Fetch  Cycle and Opcode are stored as Big Endian
	opcode = (memory[pc] << 8u) | (memory[pc+1]);

	//Increment Program counter
	pc += 2;

	// Decode & Execute the Opcode
	(this->*table[(opcode & 0xf000u) >> 12u]) ();

	//Decrease the timers of Delay and Sound if they are set

	if(delayTimer > 0){
		delayTimer--;
	}

	if(soundTimer > 0){
		delayTimer--;
	}

}

//Clear Display
void Chip8::OP_00E0(){
	memset(video,0,sizeof(video));
}

//Returns from the current subroutine and initializes PC value from stack
void Chip8::OP_00EE(){
	--sp;
	pc = stack[sp];	
}

//The Jump opcode doesn't store the return address and the opcode has the address to jump in the first 12 bits
void Chip8::OP_1nnn(){
	uint16_t address = opcode & 0x0FFFu;

	pc = address;

}

//The call opcode is similar to Jump opcode but the  return address is stored in the stack
void Chip8::OP_2nnn(){
	uint16_t address = opcode & 0x0FFFu;
	stack[sp] = pc;
	++sp;
	pc = address;
}

//Skip next instruction if Vx = kk
void Chip8::OP_3xkk(){

	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t kk = (opcode & 0x00FFu);

	if(registers[Vx] == kk){
		pc += 2;
	}
}

//Skip next instruction if Vx != kk
void Chip8::OP_4xkk(){

	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t kk = (opcode & 0x00FFu);

	if(registers[Vx] != kk){
		pc += 2;
	}
}

//Skip next instruction if Vx == Vy
void Chip8::OP_5xy0(){

	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if(registers[Vx] == registers[Vy]){
		pc += 2;
	}
}

//Load byte kk into register Vx
void Chip8::OP_6xkk(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t kk = (opcode & 0x00FFu);

	registers[Vx] = kk;
	
}

//Add Vx with kk and store it in register Vx
void Chip8::OP_7xkk(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t kk = (opcode & 0x00FFu);

	registers[Vx] = registers[Vx] + kk;

}

//Load contents of registers Vy into register Vx
void Chip8::OP_8xy0(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] = registers[Vy];
}

//Or the contents in regsiters Vx and Vy and store it in Vx
void Chip8::OP_8xy1(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
      	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] = registers[Vx] | registers [Vy];
}

//AND the contents in the registers Vx and Vy and store it in Vx
void Chip8::OP_8xy2(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] = registers[Vx] & registers[Vy];	
}

//XOR the contents in the registers Vx and Vy and store it in Vx
void Chip8::OP_8xy3(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] = registers[Vx] ^ registers[Vy];	
}
//ADD with carry sent into register VF (0xF)
void Chip8::OP_8xy4(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u; 
	uint16_t sum = registers[Vx] + registers[Vy];

	if(sum > 255){
		registers[0xF] = 0x0001;
	}
	else{
		registers[0xF] = 0x0000;
	}
	
	registers[Vx] = sum & 0xFFu;
}

//Subtract the contents in registers Vx and Vy and store the result in Vx and Not borrow in VF
void Chip8::OP_8xy5(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u; 

	if(registers[Vx] > registers[Vy]){
		registers[0xF] = 1;
	}
	else{
		registers[0xF] = 0;
	}
	
	registers[Vx] = registers[Vx] - registers[Vy];
}

//Shift register Vx by 1
void Chip8::OP_8xy6(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	registers[0xF] = (registers[Vx] & 0x1u);
	registers[Vx] = registers[Vx] >> 1u;	
}

//Subtract the contents in registers Vx and Vy and store the result in Vx 
void Chip8::OP_8xy7(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if(registers[Vy] > registers[Vx]){
		registers[0xF] = 1;
	}
	else{
		registers[0xF] = 0;
	}
	
	registers[Vx] = registers[Vy] - registers[Vx];
}

//Check the most significant bit of Vx and set that value to VF and multiply Vx by 2

void Chip8::OP_8xyE(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	//Setting the MSB to register VF	
	registers[0xF] = (registers[Vx] & 0x80u) >> 7u;

	registers[Vx] = registers[Vx] << 1; 
	
}

//Skip next instruction if register Vx and register Vy does not match

void Chip8::OP_9xy0(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if(registers[Vx] != registers[Vy]){
		pc += 2;
	}

}
//Load the address into index
void Chip8::OP_Annn(){
	uint16_t address = opcode & 0x0FFF;

	index = address;
}

//Jump to location registers[0] + nnn;
void Chip8::OP_Bnnn(){
	uint16_t address = opcode & 0x0FFF;

	pc = registers[0] + address;

}

//Set random byte & kk to registers[Vx]
void Chip8::OP_Cxkk(){
	uint8_t kk = opcode & 0x00FF;
	uint8_t Vx = (opcode & 0x0F00) >> 8u;

	registers[Vx] = randByte(randGen) & kk;
}

//Drawing the sprite from (x,y) of 8 pixels wide for a height of n
void Chip8::OP_Dxyn(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	
	uint8_t height = opcode & 0x000Fu;
	uint32_t *screen_pixel; 
	uint8_t sprite_byte;
	uint8_t sprite_pixel;
	int i,row,col;

	//Is set to 1 if there a collision between sprite_pixel and screen_pixel
	registers[0xF] = 0;

	uint8_t xPos = registers[Vx] % video_width;
       	uint8_t yPos = registers[Vy] % video_height;

	if(extended_mode && height == 0){

		for(row = 0; row < 16; row++){
			
			for(col = 0; col < 16; col++){
				
				screen_pixel = &video[(yPos + row)*video_width + (xPos + col)];
				
				// If screen pixel is ON
				if(*screen_pixel == 0xFFFFFFFF){

					registers[0xF] = 1;

				}

				//To flip the screen pixel if sprite_pixel is set
				*screen_pixel = *screen_pixel ^ 0xFFFFFFFF;
			}

		}	

		return;
	}

	for(row = 0; row < height; row++){
		
		//Each sprite is 8 bit wide (8 pixel wide) even when drawing, and its stored in memory starting from location specified in index
		sprite_byte = memory[index + row];

		for(col = 0; col < 8; col++){
			
			//Picking each pixel
			sprite_pixel = sprite_byte & (0x80u >> col);
			
			//Will return the corresponding pixel value on screen, we use 0x00000000 and 0xFFFFFFFF to determine 0 and 1, Pointer used to directly manipulate the array
			if(extended_mode){	
				screen_pixel = &video[(yPos + row)*video_width + (xPos + col)];
			} else {
				screen_pixel = &video[(2*yPos + 2*row)*video_width + (2*xPos + 2*col)];
			}
			//If sprite pixel is ON
			if(sprite_pixel){
				
				// If screen pixel is ON
				if(*screen_pixel == 0xFFFFFFFF){

					registers[0xF] = 1;

				}

				//To flip the screen pixel if sprite_pixel is set
				*screen_pixel = *screen_pixel ^ 0xFFFFFFFF;
				
				if(!extended_mode){
					video[(2*yPos + 2*row)*video_width + (2*xPos + 2*col + 1)] = *screen_pixel;
					video[(2*yPos + 2*row + 1)*video_width + (2*xPos + 2*col)] = *screen_pixel;
					video[(2*yPos + 2*row + 1)*video_width + (2*xPos + 2*col + 1)] = *screen_pixel;
				}

			}
		}

	}	

}

//Skip next instruction if key (found in registers[Vx]) is pressed on keypad
void Chip8::OP_Ex9E(){

	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	uint8_t key = registers[Vx];

	if(keypad[key]){

		pc += 2;

	}

}

//Skip next instruction if key (found in registers[Vx]) is not pressed on keypad
void Chip8::OP_ExA1(){

	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	uint8_t key = registers[Vx];

	if(!keypad[key]){

		pc += 2;

	}

}

//LD Vx, DT
//Load delay timer value in register Vx
void Chip8::OP_Fx07(){
	
	uint8_t Vx = (opcode & 0x0F00) >> 8u;

	registers[Vx] = delayTimer;

}

//LD Vx,K
//Wait for a key press and store the value of Key in Vx
void Chip8::OP_Fx0A(){
	
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	
	if(keypad[0]) {
		registers[Vx] = 0;
	} 
	else if(keypad[1]) {
		registers[Vx] = 1;
	}
	else if(keypad[2]) {
		registers[Vx] = 2;
	}
	else if(keypad[3]) {
		registers[Vx] = 3;
	}
	else if(keypad[4]) {
		registers[Vx] = 4;
	}
	else if(keypad[5]) {
		registers[Vx] = 5;
	}
	else if(keypad[6]) {
		registers[Vx] = 6;
	}
	else if(keypad[7]) {
		registers[Vx] = 7;
	}
	else if(keypad[8]) {
		registers[Vx] = 8;
	}
	else if(keypad[9]) {
		registers[Vx] = 9;
	}
	else if(keypad[10]) {
		registers[Vx] = 10;
	}
	else if(keypad[11]) {
		registers[Vx] = 11;
	}
	else if(keypad[12]) {
		registers[Vx] = 12;
	}
	else if(keypad[13]) {
		registers[Vx] = 13;
	}
	else if(keypad[14]) {
		registers[Vx] = 14;
	}
	else if(keypad[15]) {
		registers[Vx] = 15;
	}
	else {
		pc -= 2;
	}
}

//LD DT, Vx
//Set Delay Timer with value in Vx
void Chip8::OP_Fx15(){
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	delayTimer = registers[Vx];
}

//LD ST, Vx
//Set sound timer with Value in Vx
void Chip8::OP_Fx18(){
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	soundTimer = registers[Vx];
}

//Add I, Vx
//Add Vx to index register
void Chip8::OP_Fx1E(){
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	index += registers[Vx];
}

//LD F, Vx
//It sets the index register to the starting address of Sprite digit in register Vx (basically Vx value)
//Font Adddress start from 0x50 and every 5 bytes after that is a new digit
void Chip8::OP_Fx29(){
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	index = font_start_address + (5 * registers[Vx]);
}

//LD B, Vx
//Stores the hundreds position of value in Vx at I in memory, tens position at I+1 in memory, ones position at I+2 in memory
void Chip8::OP_Fx33(){
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	uint8_t value = registers[Vx];
	uint8_t hundreds_place = value/100;
	uint8_t tens_place = value/10 - (hundreds_place * 10);
	uint8_t ones_place = value - (hundreds_place * 100) - (tens_place * 10);

	memory[index] = hundreds_place;
	memory[index+1] = tens_place;
	memory[index+2] = ones_place;
}

//LD [I], Vx
//Store registers from V0 till Vx starting from Index location in Memory
void Chip8::OP_Fx55(){
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	uint8_t temp = 0;

	while(temp <= Vx) {
		memory[index+temp] = registers[temp];
		temp += 1;	
	}
}

//LD Vx, [I]
//Load registers from V0 till Vx starting from Index location in Memory
void Chip8::OP_Fx65(){
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	uint8_t temp = 0;

	while(temp <= Vx) {
		registers[temp] = memory[index + temp];
		temp += 1;	
	}
}

//Scroll down by n/2 pixels if not extended mode, else n pixels
void Chip8::OP_00Cn(){

	uint8_t pixels = (opcode & 0x000F);
	
	if(!extended_mode){
		pixels = pixels >> 1;
	}

	uint32_t i = video_width*(video_height - (pixels+1));
	uint32_t j = 0;

	for(; i>=0  && i <= video_width*(video_height-1) ; i-=video_width) {
		for(j=0; j<video_width; j++) {

			video[i + j + video_width*pixels] = video[i+j];
			if (i < video_width * pixels){
				video[i+j] = 0;
			}

		}
	}

}

void Chip8::OP_00FC(){

	uint8_t pixels = 4;
	int i,j;

	if(!extended_mode){
		pixels = 2;
	}

	for(i=0; i <= video_width*(video_height - 1) ; i+= video_width) {
		for(j=4; j < video_width; j++) {
			video[i+ (j-pixels)] = video[i+j];
			if (j >= video_width - pixels){
				video[i+j] = 0;
			}

		}
	}
/*
	for(i=0; i <= 64*31 video_width*(video_height - 1) ; i+=64 video_width) {
		for(j=4; j < 64 video_width; j++) {
			video[i+ (j-pixels)] = video[i+j];
			if (j >= 60 video_width - pixels){
				video[i+j] = 0;
			}

		}
	}
*/
}

void Chip8::OP_00FB(){

	uint8_t pixels = 4;
	int i,j;

	if(!extended_mode){
		pixels = 2;
	}

	for(i=0; i <= video_width*(video_height - 1); i+= video_width) {
		for(j=(video_width - pixels - 1); j >= 0; j--) {
			
			video[i+ j + pixels] = video[i + j];
			if (j < pixels) {
				video[i+j] = 0;
			}

		}
	}
/*
	for(i=0; i <= 64*31 video_width*(video_height - 1); i+=64 video_width) {
		for(j=59 (video_width - pixels - 1); j >= 0; j--) {
			
			video[i+ j + pixels] = video[i + j];
			if (j < 4) {
				video[i+j] = 0;
			}

		}
	}
*/
}

// Exit out of Super Chip8 Interpreter
void Chip8::OP_00FD(){
	std::exit(0);	
}

//Disable extended mode
void Chip8::OP_00FE(){

	//Have to downscale 2x2 pixel grid into 1 pixel before disabling the extended mode
/*
	int i,j,row,col;

	row = 0;
	col = 0;

	//One has to start downscaling from the bottom right corner of the video 

	for(i=0; i<=(video_height_extended-1)*(video_width_extended); i+=2*video_width_extended) {

		for(j=0; j<video_width_extended; j+=2) {
			video[row + col] = video[i + j];
			col += 1;
		}

		row += 64;
		col = 0;
	}

	video_width = video_width_normal;
	video_height = video_height_normal;
*/
	extended_mode = false;

}

//Enable extended mode
void Chip8::OP_00FF(){
/*
	//Have to upscale 1 pixel into a 2x2 pixel grid before enabling the extended mode

	int i,j;

	//One has to start  upscaling from the bottom right corner of the video 

	for(i=(video_height_normal-1)*(video_width_normal); i >= 0; i-=video_width_normal) {

		for(j=(video_width_normal-1); j >= 0; j--) {

			video[i*4 + j*2] = video[i+j];
			video[i*4 + (j*2 + 1)] = video[i+j];
			video[(i*4 + video_width_extended) +  j*2] = video[i+j];
			video[(i*4 + video_width_extended) + (j*2+1)] = video[i+j];

		}

	}


	video_width = video_width_extended;
	video_height = video_height_extended;
*/
	extended_mode = true;

}

//Load extended mode digit (stored in register Vx) font address into index register
void Chip8::OP_Fx30(){
	
	uint8_t Vx = (opcode & 0x0F00) >> 8u;

	index = extended_font_start_address + (registers[Vx] * 10);

}

//Store registers V0..Vx in RPL user flags (X<=7)
void Chip8::OP_Fx75(){

	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	uint8_t i = 0;

	for(i=0; i<=Vx; i++){
		rpl[i] = registers[i];
	}

}

//Load registers V0..Vx from RPL user flags (X<=7)
void Chip8::OP_Fx85(){

	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	uint8_t i = 0;

	for(i=0; i<=Vx; i++){
		registers[i] = rpl[i];
	}

}
