#include "chip8.h"
#include <cstdio>
//#include <GLUT/glut.h>
#include "SDL.h"
#include "SDL_opengl.h"

#define fileBufferSize 1024

uint8_t fileBuffer[fileBufferSize];

SDL_Window *sdlWindow;
SDL_Renderer *sdlRenderer;

void drawInitial(){
    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
    printf("%s\n", SDL_GetError());
 
    SDL_RenderClear(sdlRenderer);
    printf("%s\n", SDL_GetError());
    SDL_RenderPresent(sdlRenderer);
    printf("%s\n", SDL_GetError());
}

void updateScreen(uint8_t * screen){
    printf("update screen\n");
    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
    //SDL_RenderClear(sdlRenderer);
    //SDL_SetRenderDrawColor(sdlRenderer, 0, 200, 0, 255);
    for(int j=0; j<32; j++){
        for(int i=0; i<64; i++){
            if(screen[j*64 + i] == 1){
                SDL_SetRenderDrawColor(sdlRenderer, 0, 200, 0, 255);
                SDL_RenderDrawPoint(sdlRenderer, i, j);
            }
            else{
                SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
                SDL_RenderDrawPoint(sdlRenderer, i, j);
            }
        }
    }
    //resetscreen bits?
//    printf("%s\n", SDL_GetError());
    //SDL_RenderClear(sdlRenderer);
    SDL_RenderPresent(sdlRenderer);
}

int main() {
    
    int error;
    error = SDL_Init(SDL_INIT_TIMER |  SDL_INIT_VIDEO | SDL_INIT_EVENTS | \
                     SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | \
                     SDL_INIT_GAMECONTROLLER | SDL_INIT_SENSOR );//| SDL_INIT_AUDIO); // gives "no factory registered" error!!
    //if(error<0)printf("SDL_Init failed: %s\n",SDL_GetError());
//    SDL_Window *sdlWindow = SDL_CreateWindow("My Game Window",
//                                SDL_WINDOWPOS_CENTERED,
//                                SDL_WINDOWPOS_CENTERED,
//                                640, 320,
//                                SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);//SDL_WINDOW_OPENGL); not used with renderer? //
//    SDL_Renderer *sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, 0);
    SDL_CreateWindowAndRenderer(640, 320, SDL_WINDOW_RESIZABLE, &sdlWindow, &sdlRenderer);
    printf("%s\n", SDL_GetError());
    //SDL_RenderSetScale(sdlRenderer, 640, 320);
    SDL_RenderSetLogicalSize(sdlRenderer, 64, 32);
    printf("%s\n", SDL_GetError());
    
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother.
    //    SDL_CreateWindowAndRenderer(640, 480, 0, &sdlWindow, &sdlRenderer);//SDL_WINDOW_OPENGL); not used with renderer?
    if (sdlWindow == NULL || sdlRenderer == NULL) {
        printf("Failed to create window or render!");
        return 1;
    }
    drawInitial();
    
    

    FILE * programF = fopen("/Users/gorkemcaylak/Documents/Chip8/Chip8/invaders.c8", "rb");
    int bytesRead;
    if(programF != NULL){
        bytesRead = fread(fileBuffer, sizeof(fileBuffer[0]), fileBufferSize, programF);
    }
    else{
        printf("couldnt open file!!");
        return 1;
    }
    printf("bytes read: %d\n", bytesRead);
    for(int i =0; i<10; i++)
        printf("0x%X ", fileBuffer[0]);
    printf("\n");
    
    chip8 chip8_emu;
    chip8_emu.initialize();
    chip8_emu.loadProgram(fileBuffer, bytesRead);
    
    
    SDL_Event e;
    bool quit = false;
    while (!quit){
        while (SDL_PollEvent(&e)){
            if (e.type == SDL_KEYDOWN){
                /* Print the hardware scancode first */
                printf( "Scancode: 0x%02X", e.key.keysym.scancode );
                /* Print the name of the key */
                printf( ", Name: %s\n", SDL_GetKeyName( e.key.keysym.sym ) );
                chip8_emu.handleKeyPress(e.key.keysym.scancode);
            }
            else if (e.type == SDL_QUIT){
                quit = true;
            }
            chip8_emu.emulateCycle();
            if(chip8_emu.readyToDraw()){
                updateScreen(chip8_emu.getScreen());
                chip8_emu.resetDrawFlag();
            }
            
        }
    }
    SDL_Quit();
    return 0;

}
