#include "chip8.h"
#include <stdlib.h>
#include <stdio.h>

void chip8::initialize() {
  PC = 0x200;
  OP = 0;
  I  = 0;
  SP = 0;

  //will clear display, stack, registers, memory

  for(int i=0; i<80; i++) {
    memory[i] = chip8_fontset[i];
  }

  //reset timers
}


void chip8::fetch() {
  
  OP = (memory[PC] << 8) | memory[PC+1];
}

void chip8::decode_execute() {

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
          //call OP & 0x0FFF
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
      if(V[(OP & 0x0F00) >> 8] == OP & 0x00FF)
        PC += 2;
      break;

    case 0x4000:
      //neq
      if(V[(OP & 0x0F00) >> 8] != OP & 0x00FF)
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
          uint32_t a = V[(OP & 0x0F00)>>8];
          uint32_t b = V[(OP & 0x00F0)>>4];
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
          uint32_t subtrahend = V[(OP & 0x0F00)>>8];
          uint32_t minuend = V[(OP & 0x00F0)>>4];
          if(subtrahend < minuend){
            subtrahend += 0x00010000; //not necessary?
            V[15] = 0;
          } 
          else{
            V[15] = 1;
          } 
          V[(OP & 0x0F00)>>8] = (uint8_t)(subtrahend - minuend);
          break;

        case 0x0006:
          uint16_t temp = V[(OP & 0x0F00)>>8];
          V[15] = temp & 0x0001;
          V[(OP & 0x0F00)>>8] >>= 1;
          break;

        case 0x0007:
          uint32_t minuend = V[(OP & 0x0F00)>>8];
          uint32_t subtrahend = V[(OP & 0x00F0)>>4];
          if(subtrahend < minuend){
            subtrahend += 0x00010000; //not necessary?
            V[15] = 0;
          } 
          else{
            V[15] = 1;
          } 
          V[(OP & 0x0F00)>>8] = subtrahend - minuend;
          break;
        case 0x000E:
          uint16_t temp = V[(OP & 0x0F00)>>8];
          V[15] = temp & 0x8000;
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
      V[(OP & 0x0F00) >> 8] = (rand() % 0x00FF) & (OP & 0x00FF)
      break;
    case 0xD000:

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
                  if(gfx[(x + xline + ((y + yline) * 64))] == 1)
    	        V[15] = 1;     // collision                            
                  gfx[x + xline + ((y + yline) * 64)] ^= 1;
    	    }
    	  }
            }
    	 
    	drawFlag = true;
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
          V[(OP & 0x0F00) >> 8] = sound_timer;
          break;
        case 0x0015:
          delay_timer = V[(OP & 0x0F00) >> 8];
          break; 
        case 0x0018:
          sound_timer = V[(OP & 0x0F00) >> 8];
          break;   
        case 0x001E:
          I += V[(OP & 0x0F00) >> 8];
          break; 
        case 0x0029:
          //	I=sprite_addr[Vx]??
          break;            
        case 0x0033:
          memory[I]     = V[(OP & 0x0F00) >> 8] / 100;
          memory[I + 1] = (V[(OP & 0x0F00) >> 8] / 10) % 10;
          memory[I + 2] = (V[(OP & 0x0F00) >> 8] % 100) % 10;
          break;
        case 0x0055:
          for(uint16_t i=0; i<16; i++){
            memory[I+i] = V[i];
          }
          break;
        case 0x0065:
          for(uint16_t i=0; i<16; i++){
            V[i] = memory[I+i];
          }
          break;
        default:
          printf("Unknown F operation with 0x%X!\n", OP);
          break;
        } 

      }
      break;
    default:
      printf("unknown OP read: 0x%X !\n ", OP);
      break;
  } 
  PC += 2;
}

void chip8::update_timers() {
  delay_timer = (delay_timer > 0) ? delay_timer-1 : 0;
  if(sount_timer == 1) {
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
