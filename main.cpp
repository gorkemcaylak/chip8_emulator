#include "chip8.h"
#include <cstdio>
#include <GLUT/glut.h>
/*
void display() {
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
   glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer (background)
 
   // Draw a Red 1x1 Square centered at origin
   glBegin(GL_QUADS);              // Each set of 4 vertices form a quad
      glColor3f(1.0f, 0.0f, 0.0f); // Red
      glVertex2f(-0.5f, -0.5f);    // x, y
      glVertex2f( 0.5f, -0.5f);
      glVertex2f( 0.5f,  0.5f);
      glVertex2f(-0.5f,  0.5f);
   glEnd();
 
   glFlush();  // Render now
}



int main(int argc, char** argv) {
   glutInit(&argc, argv);                 // Initialize GLUT
   glutCreateWindow("OpenGL Setup Test"); // Create a window with the given title
   glutInitWindowSize(320, 320);   // Set the window's initial width & height
   glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
   glutDisplayFunc(display); // Register display callback handler for window re-paint
   glutMainLoop();           // Enter the event-processing loop
   return 0;
}
*/

#define fileBufferSize 1024

uint8_t fileBuffer[fileBufferSize];

int main() {
  chip8 chip8_emu;
  chip8_emu.initialize();

  FILE * programF = fopen("chip8.c8", "rb");
  size_t bytesRead;
  if(programF != NULL){
    bytesRead = fread(fileBuffer, sizeof(fileBuffer[0]), fileBufferSize, programF);
  }
  else{
    printf("couldnt open file!!");
    while(1);
  }
  printf("bytes read: %d\n", bytesRead);
  for(int i =0; i<10; i++)
    printf("0x%X ", fileBuffer[0]);
  printf("\n");
  //load program from image file into memory
  for(;;) {
    //chip8_emu.emulateCycle();

  }


}
