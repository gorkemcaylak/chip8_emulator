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

typedef uint16_t opcode_t;
class chip8{

  public:
    void initialize();
    void emulateCycle();

  private:
    uint8_t memory[MEMORY_SIZE];
    uint8_t V[REG_COUNT]; // V0 to VE
    uint16_t I; // index register
    uint16_t PC; // program counter
    
    uint8_t screen[64 * 32];
    uint8_t delay_timer;
    uint8_t sound_timer;
    
    uint16_t stack[STACK_SIZE];
    uint16_t SP;
    
    uint8_t keypad[16];
    
};
