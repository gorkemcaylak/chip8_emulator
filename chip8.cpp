#include chip8.h

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

  switch(OP & 0xF000) { // leftmost 4 bits used for opcode interpretation
    case 0xA000:
      I = OP & 0x0FFF;
      PC += 2;
      break;

    default:
      printf("unknown opcode read: 0x%X !\n ", OP);
      break;

  } 

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
