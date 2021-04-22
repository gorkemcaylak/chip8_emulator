#include chip8.h

int main() {

  chip8 chip8_emu;
  chip8_emu.initialize();
  //load program from image file into memory
  for(;;) {
    chip8_emu.emulateCycle();

  }



}
