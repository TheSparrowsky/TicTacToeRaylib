#include <iostream>
#include "raylib.h"

int main(int argc, char** argv)
{
  InitWindow(800, 600, "Hello World");
  while(!WindowShouldClose())
  {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Hello World!", 350, 280, 20, BLACK);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
