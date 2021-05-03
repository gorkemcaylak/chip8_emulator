#include "chip8.h"
#include <stdlib.h>
#include <stdio.h>
#include <unordered_map>

uint8_t chip8_fontset[80] =
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
/*
 Scancode: 0x1E, Name: 1
 Scancode: 0x1F, Name: 2
 Scancode: 0x20, Name: 3
 Scancode: 0x21, Name: 4
 Scancode: 0x14, Name: Q
 Scancode: 0x1A, Name: W
 Scancode: 0x08, Name: E
 Scancode: 0x15, Name: R
 Scancode: 0x04, Name: A
 Scancode: 0x16, Name: S
 Scancode: 0x07, Name: D
 Scancode: 0x09, Name: F
 Scancode: 0x1D, Name: Z
 Scancode: 0x1B, Name: X
 Scancode: 0x06, Name: C
 Scancode: 0x19, Name: V
 
 */
std::unordered_map<int, int> keyMap = {
    {0x1E, 0}, //1
    {0x1F, 1}, //2
    {0x20, 2}, //3
    {0x21, 3}, //4
    {0x14, 4}, //q
    {0x1A, 5}, //w
    {0x08, 6}, //e
    {0x15, 7}, //r
    {0x04, 8}, //a
    {0x16, 9}, //s
    {0x07,10}, //d
    {0x09,11}, //f
    {0x1D,12}, //z
    {0x1B,13}, //x
    {0x06,14}, //c
    {0x19,15}  //v
};

void chip8::handleKeyPress(int key) {
    if(keyMap.count(key) > 0){
        keypad[keyMap[key]] = 1;
        printf("key: 0x%X\n", key);

    }
    else {
        printf("undefined key operation!\n");
    }
}

void chip8::loadProgram(uint8_t * buffer, int size) {
    for(int i=0; i<size; i++) {
        memory[0x200+i] = buffer[i];
    }
}

chip8::chip8() {
    
}

chip8::~chip8() {
    
}
void chip8::initialize() {
    PC = 0x200;
    OP = 0;
    I  = 0;
    SP = 0;

    //will clear display, stack, registers, memory
    for(int i=0; i<64*32;i++) {
        screen[i] = 0;
    }
//    for(int j=0; j<32; j++){
//        for(int i=0; i<64; i++){
//            if(i<(j*2)) screen[i+j*64] = 1;
//        }
//    }
            
    for(int i = 0; i < MEMORY_SIZE; i++)
        memory[i] = 0;
    
    for(int i = 0; i < 80; i++)
        memory[i] = chip8_fontset[i];
    
    for(int i = 0; i < STACK_SIZE; i++)
        stack[i] = 0;
    
    for(int i = 0; i < REG_COUNT; i++)
        V[i] = 0;
    
    for(int i = 0; i < 16; i++)
        keypad[i] = 0;
    
    delay_timer = 0;
    sound_timer = 0;
    
    drawFlag = true;
    printf("chip 8 emulator initialized\n");
    //reset timers
    srand ((int)time(NULL));
}


void chip8::fetch() {
  
    OP = (memory[PC] << 8) | memory[PC+1];
    printf("OP : 0x%.4X\n", OP);
}

void chip8::decode_execute() {

  uint32_t a = V[(OP & 0x0F00)>>8];
  uint32_t b = V[(OP & 0x00F0)>>4];
  switch(OP & 0xF000) { // leftmost 4 bits used for OP interpretation
    case 0x0000:
      switch(OP) {
        case 0x00E0:
          //display clear
          for(int i=0; i<64*32; i++) 
            screen[i] = 0;
          drawFlag = true;
          // int r = 0, g = 128, b = 0; //green
          // SDL_FillRect(screen_surface, NULL, SDL_MapRGB(SDL_GetVideoSurface()->format, r,g,b));
          break;
        case 0x00EE:
          SP--;
          PC = stack[SP];
          //return
          break;
        default:
          //call OP & 0x0FFF jump to a machine code routine
              printf("SHOULDNT BE HERE!!");
              while(1);
          break; 
      }
      break;

    case 0x1000:
      //goto OP & 0x0FFF
      PC = OP & 0x0FFF;
      PC -= 2;
      break;

    case 0x2000:
      //calls subroutine OP & 0x0FFF
      stack[SP] = PC;
      SP++;
      PC = OP & 0x0FFF;
      PC -= 2; 
      break;

    case 0x3000:
      //eq
      if(V[(OP & 0x0F00) >> 8] == (OP & 0x00FF))
        PC += 2;
      break;

    case 0x4000:
      //neq
      if(V[(OP & 0x0F00) >> 8] != (OP & 0x00FF))
        PC += 2;
      break;

    case 0x5000:
      //compare if two regs equal
      if(V[(OP & 0x0F00) >> 8] == V[(OP & 0x00F0) >> 4])
        PC += 2;
      break;

    case 0x6000:
      V[(OP & 0x0F00)>>8] = OP & 0x00FF;
      break;

    case 0x7000:
      V[(OP & 0x0F00)>>8] += OP & 0x00FF;
      break; 

    case 0x8000:
      switch(OP & 0x000F){
        case 0x0000:
          V[(OP & 0x0F00)>>8] = V[(OP & 0x00F0)>>4];
	        break;

        case 0x0001:
          V[(OP & 0x0F00)>>8] |= V[(OP & 0x00F0)>>4];
          break;

        case 0x0002:
          V[(OP & 0x0F00)>>8] &= V[(OP & 0x00F0)>>4];
          break;

        case 0x0003:
          V[(OP & 0x0F00)>>8] ^= V[(OP & 0x00F0)>>4];
          break;

        case 0x0004:
          V[(OP & 0x0F00)>>8] += V[(OP & 0x00F0)>>4];
          a += b;
          if(a > 0x0000FFFF){
            a -= 0x00010000; //not necessary?
            V[15] = 1;
          } 
          else{
            V[15] = 0;
          } 
          V[(OP & 0x0F00)>>8] = (uint8_t)a;
          break;

        case 0x0005:
//          uint32_t subtrahend = V[(OP & 0x0F00)>>8];
//          uint32_t minuend = V[(OP & 0x00F0)>>4];
          if(a < b){
            a += 0x00010000; //not necessary?
            V[15] = 0;
          } 
          else{
            V[15] = 1;
          } 
          V[(OP & 0x0F00)>>8] = (uint8_t)(a - b);
          break;

        case 0x0006:
//          uint16_t temp = V[(OP & 0x0F00)>>8];
          V[15] = a & 0x0001;
          V[(OP & 0x0F00)>>8] >>= 1;
          break;

        case 0x0007:
//          uint32_t minuend = V[(OP & 0x0F00)>>8];
//          uint32_t subtrahend = V[(OP & 0x00F0)>>4];
          if(b < a){
            b += 0x00010000; //not necessary?
            V[15] = 0;
          } 
          else{
            V[15] = 1;
          } 
          V[(OP & 0x0F00)>>8] = b - a;
          break;
        case 0x000E:
//          uint16_t temp = V[(OP & 0x0F00)>>8];
          V[15] = a & 0x8000;
          V[(OP & 0x0F00)>>8] <<= 1;
          break;
        default:
          printf("Unknown math operation with 0x%X!\n", OP);
          break;
        } 
    case 0x9000:
      //compare 2 regs not equal
      if(V[(OP & 0x0F00) >> 8] != V[(OP & 0x00F0) >> 4])
        PC += 2;
      break;
    case 0xA000:
      I = OP & 0x0FFF;
      break;
    case 0xB000:
      PC = V[0] + (OP & 0x0FFF); 
      PC -= 2; 
      break;
    case 0xC000:
          V[(OP & 0x0F00) >> 8] = (rand() % 0x00FF) & (OP & 0x00FF);
      break;
    case 0xD000:
      {
    //Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N+1 pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; I value does not change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that does not happen
            //starting from memory[I]
    	uint16_t x = V[(OP & 0x0F00) >> 8];
    	uint16_t y = V[(OP & 0x00F0) >> 4];
    	uint16_t height = OP & 0x000F;
        uint16_t pixel;
    	 
    	V[15] = 0;
    	for (int yline = 0; yline < height; yline++)
    	{
    	  pixel = memory[I + yline];
    	  for(int xline = 0; xline < 8; xline++)
    	  {
    	    if((pixel & (0x80 >> xline)) != 0)
    	    {
                  if(screen[(x + xline + ((y + yline) * 64))] == 1)
    	        V[15] = 1;     // collision                            
                  screen[x + xline + ((y + yline) * 64)] ^= 1;
    	    }
    	  }
            }
    	 
    	drawFlag = true;
      }
    	break;
    case 0xE000:
      switch(OP & 0x00FF){
        case 0x009E:  
          if(keypad[V[(OP & 0x0F00) >> 8]] == 0) {
            PC += 2;
          }
          break;
        case 0x00A1:
          if(keypad[V[(OP & 0x0F00) >> 8]] != 0) {
            PC += 2;
          }
          break;
        default:
          printf("Unknown key operation with 0x%X!\n", OP);
          break;
      } 
    case 0xF000:
      switch(OP & 0x00FF){
        case 0x0007:
          V[(OP & 0x0F00) >> 8] = delay_timer;
          break;
        case 0x000A:
        {
          bool keyPress = false;
          for(int i = 0; i < 16; ++i) {
              if(keypad[i] != 0) {
                  V[(OP & 0x0F00) >> 8] = i;
                  keyPress = true;
              }
          }
          if(!keyPress) {
              //no key press, rerun
              PC -= 2;
              return;
          }
          break;
        }
        case 0x0015:
          delay_timer = V[(OP & 0x0F00) >> 8];
          break; 
        case 0x0018:
          sound_timer = V[(OP & 0x0F00) >> 8];
          break;   
        case 0x001E:
              if(I + V[(OP & 0x0F00) >> 8] > 0xFFF)
                  V[15] = 1;
              else
                  V[15] = 0;
              I += V[(OP & 0x0F00) >> 8];
              break;
        case 0x0029:
              I = V[(OP & 0x0F00) >> 8] * 0x0005;
          //	I=sprite_addr[Vx]??
          break;            
        case 0x0033:
          memory[I]     = V[(OP & 0x0F00) >> 8] / 100;
          memory[I + 1] = (V[(OP & 0x0F00) >> 8] / 10) % 10;
          memory[I + 2] = (V[(OP & 0x0F00) >> 8] % 100) % 10;
          break;
        case 0x0055:
          for(uint16_t i = 0; i < ((OP & 0x0F00) >> 8); i++){
            memory[I + i] = V[i];
          }
              I += ((OP & 0x0F00) >> 8) + 1;
          break;
        case 0x0065:
          for(uint16_t i = 0; i < ((OP & 0x0F00) >> 8); i++){
            V[i] = memory[I + i];
          }
              I += ((OP & 0x0F00) >> 8) + 1;
          break;
        default:
          printf("Unknown F operation with 0x%X!\n", OP);
          break;
        }
//      break;
    default:
      printf("unknown OP read: 0x%X !\n ", OP);
      break;
  } 
  PC += 2;
}

void chip8::update_timers() {
  delay_timer = (delay_timer > 0) ? delay_timer-1 : 0;
  if(sound_timer == 1) {
    printf("BEEP!\n");
    //put a sound here!
  }
  sound_timer = (sound_timer > 0) ? sound_timer-1 : 0;
}

void chip8::emulateCycle() {
  fetch();
  decode_execute();
  update_timers(); 
}
