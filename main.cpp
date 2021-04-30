#include "chip8.h"
#include <cstdio>
//#include <GLUT/glut.h>
#include "SDL.h"
#include "SDL_opengl.h"

#define fileBufferSize 1024

uint8_t fileBuffer[fileBufferSize];

//static void setOpenGLAttributes() {
//    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
//    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
//    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
//    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
//    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
//}

SDL_Window *sdlWindow;
SDL_Renderer *sdlRenderer;

void draw(){
    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
    SDL_RenderClear(sdlRenderer);
    SDL_RenderPresent(sdlRenderer);
}

int main() {
    
    int error;
    error = SDL_Init(SDL_INIT_TIMER |  SDL_INIT_VIDEO | SDL_INIT_EVENTS | \
                     SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | \
                     SDL_INIT_GAMECONTROLLER | SDL_INIT_SENSOR );//| SDL_INIT_AUDIO); // gives "no factory registered" error!!
    //if(error<0)printf("SDL_Init failed: %s\n",SDL_GetError());
    //setOpenGLAttributes(); //not necessary?
    SDL_Window *sdlWindow = SDL_CreateWindow("My Game Window",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              640, 480,
                                             SDL_WINDOW_OPENGL           );//SDL_WINDOW_OPENGL); not used with renderer?
    SDL_Renderer *sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, 0);
//    SDL_CreateWindowAndRenderer(640, 480, 0, &sdlWindow, &sdlRenderer);//SDL_WINDOW_OPENGL); not used with renderer?
    if (sdlWindow == NULL || sdlRenderer == NULL) {
        printf("Failed to create window or render!");
        return 1;
    }
    
    chip8 chip8_emu;
    chip8_emu.initialize();

    FILE * programF = fopen("/Users/gorkemcaylak/Documents/Chip8/Chip8/pong2.c8", "rb");
    size_t bytesRead;
    if(programF != NULL){
        bytesRead = fread(fileBuffer, sizeof(fileBuffer[0]), fileBufferSize, programF);
    }
    else{
        printf("couldnt open file!!");
        return 1;
    }
    printf("bytes read: %lu\n", bytesRead);
    for(int i =0; i<10; i++)
        printf("0x%X ", fileBuffer[0]);
    printf("\n");
    
    draw();
    SDL_Event e;
    bool quit = false;
    while (!quit){
        while (SDL_PollEvent(&e)){
            if (e.type == SDL_QUIT){
                quit = true;
            }
        }
    }

}
