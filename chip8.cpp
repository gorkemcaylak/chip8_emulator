#include "chip8.h"
#include <cstdlib>
#include <cstdio>
#include <unordered_map>
#include "SDL.h"


#define FILE_BUFFER_SIZE 2048


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

void chip8::handleKeyPress(int key, bool down) {
    switch(key){
        case SDL_SCANCODE_1:
            keypad[0x1] = down;
            break;
        case SDL_SCANCODE_2:
            keypad[0x2] = down;
            break;
        case SDL_SCANCODE_3:
            keypad[0x3] = down;
            break;
        case SDL_SCANCODE_4:
            keypad[0xC] = down;
            break;
        case SDL_SCANCODE_Q:
            keypad[0x4] = down;
            break;
        case SDL_SCANCODE_W:
            keypad[0x5] = down;
            break;
        case SDL_SCANCODE_E:
            keypad[0x6] = down;
            break;
        case SDL_SCANCODE_R:
            keypad[0xD] = down;
            break;
        case SDL_SCANCODE_A:
            keypad[0x7] = down;
            break;
        case SDL_SCANCODE_S:
            keypad[0x8] = down;
            break;
        case SDL_SCANCODE_D:
            keypad[0x9] = down;
            break;
        case SDL_SCANCODE_F:
            keypad[0xE] = down;
            break;
        case SDL_SCANCODE_Z:
            keypad[0xA] = down;
            break;
        case SDL_SCANCODE_X:
            keypad[0x0] = down;
            break;
        case SDL_SCANCODE_C:
            keypad[0xB] = down;
            break;
        case SDL_SCANCODE_V:
            keypad[0xF] = down;
            break;
        case SDL_SCANCODE_ESCAPE:
            printf("Quitting...\n");
            exit(1);
            break;
        default:
            printf("undefined key operation!\n");
            break;
    }
}

void chip8::loadProgram(const char * rom_name) {
    FILE * programF = fopen(rom_name, "rb");
    int bytesRead;
    if(programF != NULL){
        bytesRead = (int)fread(&memory[0x200], sizeof(uint8_t), FILE_BUFFER_SIZE, programF);
        if(bytesRead == -1) {
            printf("couldnt read!!");
            exit(1);
        }
    }
    else{
        printf("couldnt open file!!");
        exit(1);
    }
    
    printf("bytes read: %d\n", bytesRead);
    for(int i =0; i<10; i++)
        printf("0x%X ", memory[0x200 + i]);
    printf("\n");
}

chip8::chip8() {}

chip8::~chip8() {}

void chip8::initialize() {
    PC = 0x200;
    OP = 0;
    I  = 0;
    SP = 0;

    //will clear display, stack, registers, memory
    for(int i=0; i<SCREEN_W*SCREEN_H;i++) {
        screen[i] = OFF;
    }
    
    for(int i = 0; i < MEMORY_SIZE; i++)
        memory[i] = 0;
//    memset(memory, 0, sizeof(memory));
    
    for(int i = 0; i < FONTSET_ADDRESS; i++)
        memory[i] = chip8_fontset[i];

    
    for(int i = 0; i < STACK_SIZE; i++)
        stack[i] = 0;

    for(int i = 0; i < REG_COUNT; i++)
        V[i] = 0;
//    memset(V, 0, sizeof(V));

    for(int i = 0; i < 16; i++)
        keypad[i] = 0;
//    memset(keypad, 0, sizeof(keypad));

    delay_timer = 0;
    sound_timer = 0;
    
    drawFlag = true;
    printf("chip 8 emulator initialized\n");
    //reset timers
    srand ((int)time(NULL));
}


void chip8::fetch() {
  
    OP = (memory[PC] << 8) | memory[PC+1];
//    printf("OP : 0x%.4X\n", OP);
}

void chip8::decode_execute() {

  uint32_t Vx = V[(OP & 0x0F00)>>8];
  uint32_t Vy = V[(OP & 0x00F0)>>4];

    PC += 2;
    
  switch(OP & 0xF000) { // leftmost 4 bits used for OP interpretation
          
      case 0x0000:
      switch(OP) {
        case 0x00E0:
          //display clear
          for(int i=0; i<SCREEN_W*SCREEN_H; i++)
            screen[i] = OFF;
//          drawFlag = true;
          // int r = 0, g = 128, b = 0; //green
          // SDL_FillRect(screen_surface, NULL, SDL_MapRGB(SDL_GetVideoSurface()->format, r,g,b));
          break;
        case 0x00EE:
              SP--;
              if(SP < 0){
                  printf("stack underflow!\n");
                  exit(1);
              }
          
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
//      PC -= 2;
      break;

    case 0x2000:
      //calls subroutine OP & 0x0FFF
      stack[SP] = PC;
      SP++;
          if(SP > 15){
              printf("stack overflow!\n");
              exit(1);
          }
      
      PC = OP & 0x0FFF;
//      PC -= 2;
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
//          V[(OP & 0x0F00)>>8] += V[(OP & 0x00F0)>>4];
          Vx += Vy;
          if(Vx > 0x0000FFFF){
//            a -= 0x00010000; //not necessary?
            V[15] = 1;
          } 
          else{
            V[15] = 0;
          } 
          V[(OP & 0x0F00)>>8] = (uint8_t)(Vx & 0x0000FFFF);
          break;

        case 0x0005:
          if(Vx < Vy){ //equals?
            V[15] = 0;
          } 
          else{
            V[15] = 1;
          } 
          V[(OP & 0x0F00)>>8] = (uint8_t)((Vx - Vy) & 0x0000FFFF);
          break;

        case 0x0006:
          V[15] = Vx & 0x0001;
          V[(OP & 0x0F00)>>8] >>= 1;
          break;

        case 0x0007:
          if(Vy < Vx){
            V[15] = 0;
          } 
          else{
            V[15] = 1;
          } 
              V[(OP & 0x0F00)>>8] = (uint8_t)((Vy - Vx) & 0x0000FFFF);
          break;
        case 0x000E:
          V[15] = Vx >> 7;
          V[(OP & 0x0F00)>>8] <<= 1;
          break;
        default:
          printf("Unknown math operation with 0x%X!\n", OP);
          break;
        } 
    case 0x9000:
      //compare 2 regs not equal
      if(Vx != Vy)
        PC += 2;
      break;
    case 0xA000:
      I = OP & 0x0FFF;
      break;
    case 0xB000:
      PC = V[0] + (OP & 0x0FFF); 
      break;
    case 0xC000:
          V[(OP & 0x0F00) >> 8] = (rand() % 0x00FF) & (OP & 0x00FF);
      break;
    case 0xD000:
        {
            //Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N+1 pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; I value does not change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that does not happen
            //starting from memory[I]
            uint16_t Vx = V[(OP & 0x0F00) >> 8];
            uint16_t Vy = V[(OP & 0x00F0) >> 4];
            uint16_t height = OP & 0x000F;
            uint16_t pixel;

            V[15] = 0;
            for (int yline = 0; yline < height; yline++) {
                uint8_t row = yline + Vy;

                for(int xline = 0; xline < 8; xline++) {
                    pixel = memory[I + yline];
                    uint8_t col = xline + Vx;
                    if((pixel & (0x80 >> xline)) != 0)
                    {
                        int index = col % SCREEN_W + ((row % SCREEN_H) * SCREEN_W);
                        if(screen[index] == ON) {
                            V[15] = 1;     // erase
                            screen[index] = OFF;
                        }
                        else {
                            screen[index] = ON;
                        }
                    }
                }
            }
             
            drawFlag = true;
        }
        break;
          
    case 0xE000:
        switch(OP & 0x00FF){
            case 0x009E:
                if(keypad[V[(OP & 0x0F00) >> 8]] != 0)
                    PC += 2;
                break;
            case 0x00A1:
                if(keypad[V[(OP & 0x0F00) >> 8]] == 0)
                    PC += 2;
                break;
            default:
                printf("Unknown key operation with 0x%X!\n", OP);
                break;
        }
        break;
          
    case 0xF000:
        switch(OP & 0x00FF){
            case 0x0007:
            V[(OP & 0x0F00) >> 8] = delay_timer;
            break;
        case 0x000A:
        {
            bool keyPress = false;
            printf("checking key\n");
            for(int i = 0; i < 16; ++i) {
                if(keypad[i] != 0) {
                    V[(OP & 0x0F00) >> 8] = i;
                    keyPress = true;
                    printf("key press! \n");
                }
            }
            if(!keyPress) {
                //no key press, rerun
                PC -= 2;
                return;
            }
        }
            break;
        case 0x0015:
          delay_timer = V[(OP & 0x0F00) >> 8];
          break; 
        case 0x0018:
          sound_timer = V[(OP & 0x0F00) >> 8];
          break;   
        case 0x001E:
            // VF not affected!
            I += V[(OP & 0x0F00) >> 8];
            break;
        case 0x0029:
            I = V[(OP & 0x0F00) >> 8] * 5;
            break;
        case 0x0033:
          memory[I]     = Vx / 100;
          memory[I + 1] = (Vx / 10) % 10;
          memory[I + 2] = Vx % 10;
          break;
        case 0x0055:
//          for(uint16_t i = 0; i <= ((OP & 0x0F00) >> 8); i++){
//            memory[I + i] = V[i];
//          }
              memcpy(memory+I, V, ((OP & 0x0F00) >> 8) + 1);
//              I += ((OP & 0x0F00) >> 8) + 1;
          break;
        case 0x0065:
//          for(uint16_t i = 0; i <= ((OP & 0x0F00) >> 8); i++){
//            V[i] = memory[I + i];
//          }
              memcpy(V, memory+I, ((OP & 0x0F00) >> 8) + 1);
//              I += ((OP & 0x0F00) >> 8) + 1;
          break;
        default:
          printf("Unknown F operation with 0x%X!\n", OP);
          break;
        }
      break;
    default:
      printf("unknown OP read: 0x%X !\n ", OP);
      break;
  } 
  
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
//    // Draw
//        for(int y = 0; y < 32; ++y)
//        {
//            for(int x = 0; x < 64; ++x)
//            {
//                if(screen[(y*64) + x] == OFF)
//                    printf("O");
//                else
//                    printf(" ");
//            }
//            printf("\n");
//        }
//        printf("\n");
}
