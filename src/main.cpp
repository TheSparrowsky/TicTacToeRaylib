#include <iostream>
#include "raylib.h"

#define BG_COLOR DARKGRAY
#define FG_COLOR LIGHTGRAY

#define SCREEN_HEIGHT 360
#define SCREEN_WIDTH 640

#define CELL_WIDTH 80
#define CELL_HEIGHT 90 
//CELL_WIDTH

enum class Direction : char
{
  None = 0,
  North = 1,
  South = 2,
  East = 4,
  West = 8
};

constexpr Direction operator|(Direction lhs, Direction rhs)
{
  return static_cast<Direction>(static_cast<char>(lhs) | static_cast<char>(rhs));
}

constexpr Direction operator&(Direction lhs, Direction rhs)
{
  return static_cast<Direction>(static_cast<char>(lhs) & static_cast<char>(rhs));
}

constexpr bool HasFlag(Direction value, Direction flag)
{
  return (value & flag) != Direction::None;
}

struct BoardCell
{
  unsigned int Index;
  Direction WallAtDirections;

  constexpr static unsigned int Width = 80;
  constexpr static unsigned int Height = 90;

  Vector2 Position;

  void Draw(const Vector2& startPosition)
  {
    Position = startPosition;

    if(HasFlag(WallAtDirections, Direction::North))
      DrawLineEx(startPosition, {startPosition.x + Width, startPosition.y}, 2.0f, FG_COLOR);
    if(HasFlag(WallAtDirections, Direction::South))
      DrawLineEx({startPosition.x, startPosition.y + Height}, {startPosition.x + Width, startPosition.y + Height}, 2.0f, FG_COLOR);
    if(HasFlag(WallAtDirections, Direction::East))
      DrawLineEx({startPosition.x + Width, startPosition.y}, {startPosition.x + Width, startPosition.y + Height}, 2.0f, FG_COLOR);
    if(HasFlag(WallAtDirections, Direction::West))
      DrawLineEx(startPosition, {startPosition.x, startPosition.y + Height}, 2.0f, FG_COLOR);

    if(MouseOverlaps(GetMousePosition()))
      DrawRectangleRounded({Position.x+4, Position.y+4, Width - 8, Height - 8}, 0.4f, 1, VIOLET);

  }

  bool MouseOverlaps(Vector2 mousePosition)
  {
    return mousePosition.x >= Position.x && mousePosition.x <= Position.x + Width 
      && mousePosition.y >= Position.y && mousePosition.y <= Position.y + Height;
  }
};

constexpr static Direction Cells[9] =  
{ 
  Direction::South | Direction::East, Direction::South, Direction::West | Direction::South,
  Direction::South | Direction::East, Direction::None, Direction::West | Direction::South,
  Direction::East, Direction::North, Direction::West
};

int main(int argc, char** argv)
{
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello World");
  while(!WindowShouldClose())
  {
    BeginDrawing();
    ClearBackground(BG_COLOR);

    for(int i = 0; i < 9; i++)
    {
      BoardCell cell = { static_cast<unsigned int>(i), Cells[i] };
      Vector2 position = {SCREEN_WIDTH / 2.0f - cell.Width * 1.5f, 60};
      position.x += (float)cell.Width * (i % 3);
      position.y += (float)cell.Height * (int)(i / 3);
      cell.Draw(position);
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
