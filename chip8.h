#include <cinttypes>
#include <cstdbool>


// 35 opcodes
// all 2 bytes long
// 15 8-bit registers V0 to VE
// VF used for carry flag

// 0x000-0x1FF - chip8 interpreter (contains font set in emu)
// 0x050-0x0A0 - used for built-in 4x5 pixel font set (0-F)
// 0x200-0xFFF - program ROM and working RAM

#define MEMORY_SIZE 4096
#define REG_COUNT 16
#define STACK_SIZE 16

class chip8{

  public:
    chip8();
    ~chip8();
    void initialize();
    void emulateCycle();

  private:
    uint16_t OP;
    uint8_t memory[MEMORY_SIZE];
    uint8_t V[REG_COUNT]; // V0 to VE
    uint16_t I; // index register
    uint16_t PC; // program counter
    
    uint8_t screen[64 * 32];
    uint8_t delay_timer;
    uint8_t sound_timer;
    
    uint16_t stack[STACK_SIZE];
    uint16_t SP;

    bool drawFlag = false;
    
    uint8_t keypad[16];
    void fetch();
    void decode_execute();
    void update_timers(); 
};

unsigned char chip8_fontset[80] =
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

