#include "chip8.h"
//#include <GLUT/glut.h>
#include "SDL.h"
#include "SDL_opengl.h"


#define CYCLE_PERIOD (1000/60)

#define KEY_DOWN 1
#define KEY_UP 0

//uint8_t fileBuffer[FILE_BUFFER_SIZE];

SDL_Window * sdlWindow;
SDL_Renderer * sdlRenderer;
SDL_Texture * sdlTexture;

int last_frame;

void drawInitial(){
    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
    printf("%s\n", SDL_GetError());
 
    SDL_RenderClear(sdlRenderer);
    printf("%s\n", SDL_GetError());
    SDL_RenderPresent(sdlRenderer);
    printf("%s\n", SDL_GetError());
}

void updateScreen(uint32_t * screen){
//    printf("update screen\n");
    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
    SDL_RenderClear(sdlRenderer);
    SDL_SetRenderDrawColor(sdlRenderer, 0, 200, 0, 255);
    SDL_UpdateTexture(sdlTexture, NULL, screen, 64 * sizeof(uint32_t));
    SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);

    SDL_RenderPresent(sdlRenderer);
}

int main() {
    
    int error;
    error = SDL_Init(SDL_INIT_TIMER |  SDL_INIT_VIDEO | SDL_INIT_EVENTS | \
                     SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | \
                     SDL_INIT_GAMECONTROLLER | SDL_INIT_SENSOR );//| SDL_INIT_AUDIO); // gives "no factory registered" error!!
    if(error<0)
        printf("SDL_Init failed: %s\n",SDL_GetError());

    SDL_CreateWindowAndRenderer(640, 320, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN, &sdlWindow, &sdlRenderer);
    sdlTexture = SDL_CreateTexture(sdlRenderer,
                                   SDL_PIXELFORMAT_ARGB8888,
                                   SDL_TEXTUREACCESS_STATIC,
                                   64, 32);
    printf("%s\n", SDL_GetError());
    SDL_RenderSetLogicalSize(sdlRenderer, 64, 32);
    printf("%s\n", SDL_GetError());
    
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother.

    if (sdlWindow == NULL || sdlRenderer == NULL) {
        printf("Failed to create window or render!");
        return 1;
    }
    drawInitial();
    
    chip8 chip8_emu;
    chip8_emu.initialize();
    chip8_emu.loadProgram("/Users/gorkemcaylak/Documents/Chip8/Chip8/invaders.c8");
    
    SDL_Event e;
    bool quit = false;
    while (!quit){
        last_frame = SDL_GetTicks();
        if (SDL_PollEvent(&e)){
            if (e.type == SDL_KEYDOWN){
                chip8_emu.handleKeyPress(e.key.keysym.scancode, KEY_DOWN);
            }
            else if (e.type == SDL_KEYUP){
                chip8_emu.handleKeyPress(e.key.keysym.scancode, KEY_UP);
            }
            else if (e.type == SDL_QUIT){
                quit = true;
            }
            
            
        }
        chip8_emu.emulateCycle();
        if(chip8_emu.readyToDraw()){
            updateScreen(chip8_emu.getScreen());
            chip8_emu.resetDrawFlag();
        }
        int currentFrame = SDL_GetTicks() - last_frame;
        if(currentFrame < CYCLE_PERIOD) {
            SDL_Delay(CYCLE_PERIOD - currentFrame);
        }
    }
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
    return 0;

}
