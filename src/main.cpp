#include <iostream>
#include <tuple>
#include <functional>
#include <array>
#include "raylib.h"

#define BG_COLOR DARKGRAY
#define FG_COLOR LIGHTGRAY

#define SCREEN_HEIGHT 360
#define SCREEN_WIDTH 640

#define CELL_WIDTH 80
#define CELL_HEIGHT 90 
//CELL_WIDTH

bool g_GameOver = false;

struct Button 
{
  Rectangle Bounds;
  const char* Text;
  std::function<void()> OnClick;

  Color m_Color;

  void Update()
  {
    if(CheckCollisionPointRec(GetMousePosition(), Bounds))
    {
      m_Color = DARKGRAY;
      if(IsMouseButtonPressed(0))
        if(OnClick)
          OnClick();
    }
    else
    {
      m_Color = WHITE;
    }
  }

  void Draw() const
  {
    int textWidth = MeasureText(Text, 40.f);
    DrawRectangleRec(Bounds, FG_COLOR);
    DrawText(Text, Bounds.x + (Bounds.width / 2.f - textWidth / 2.f), Bounds.y + (Bounds.height / 2.f - 20.f), 40.f, m_Color);
  }

};

enum class CellState : uint8_t
{
  None = 0,
  X,
  O
};

static CellState g_PlayerTurn = CellState::O;

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
  CellState State;

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

    if(State == CellState::O)
      DrawCircleLines((Position.x + Width / 2.0f), (Position.y + Height/2.0f), 20, FG_COLOR);
    else if(State == CellState::X)
    {
      DrawLineEx({startPosition.x + 20, startPosition.y + 20}, {startPosition.x + Width - 20, startPosition.y + Height - 20}, 2.0f, FG_COLOR);
      DrawLineEx({startPosition.x + Width - 20, startPosition.y + 20}, {startPosition.x + 20, startPosition.y + Height - 20}, 2.0f, FG_COLOR);
    }

    if(!g_GameOver && MouseOverlaps(GetMousePosition()) && State == CellState::None)
    {
      DrawRectangleRounded({Position.x+4, Position.y+4, Width - 8, Height - 8}, 0.4f, 1, VIOLET);
      if(IsMouseButtonPressed(0))
      {
        State = g_PlayerTurn;
        g_PlayerTurn = g_PlayerTurn == CellState::O ? CellState::X : CellState::O;
      }
    }
  }

  bool MouseOverlaps(Vector2 mousePosition)
  {
    return mousePosition.x >= Position.x && mousePosition.x <= Position.x + Width 
      && mousePosition.y >= Position.y && mousePosition.y <= Position.y + Height;
  }
};

//constexpr static Direction Cells[9] =  
//{ 
//  Direction::South | Direction::East, Direction::South, Direction::West | Direction::South,
//  Direction::South | Direction::East, Direction::None, Direction::West | Direction::South,
//  Direction::East, Direction::North, Direction::West
//};

static std::array<BoardCell, 9> Cells; 

static void ClearBoard()
{
  Cells =
  {
    BoardCell{0, Direction::South | Direction::East }, {1, Direction::South}, {2, Direction::West | Direction::South},
    {3, Direction::South | Direction::East }, {4, Direction::None }, {5, Direction::West | Direction::South},
    {6, Direction::East}, {7, Direction::North}, {8, Direction::West}
  };
}

constexpr static std::tuple<int, int, int> WinIndexCombinations[8] =
{
  {0, 1, 2}, {3, 4, 5}, {6, 7, 8},
  {0, 3, 6}, {1, 4, 7}, {2, 5, 8},
  {0, 4, 8}, {2, 4, 6}
};

bool IsAnyMovePossible()
{
  for(const auto& cell : Cells)
    if(cell.State == CellState::None)
      return true;

  return false;
}

const char* PlayerTurnToString()
{
  switch(g_PlayerTurn)
  {
    case CellState::X:
      return "X's turn";
    case CellState::O:
      return "O's turn";
    default:
      return "";
  }
}

int main(int argc, char** argv)
{
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tic Tac Toe");

  bool closeWindow = false;
  ClearBoard();
  while(!closeWindow)
  {
    closeWindow = WindowShouldClose();

    if(!IsAnyMovePossible())
      g_GameOver = true;

    BeginDrawing();
    ClearBackground(BG_COLOR);

    for(int i = 0; i < 9; i++)
    {
      BoardCell& cell = Cells[i];// { static_cast<unsigned int>(i), Cells[i] };
      Vector2 position = {SCREEN_WIDTH / 2.0f - cell.Width * 1.5f, 60};
      position.x += (float)cell.Width * (i % 3);
      position.y += (float)cell.Height * (int)(i / 3);
      cell.Draw(position);
    }

    for(int i = 0; i < 8; i++)
    {
      const auto& [a, b, c] = WinIndexCombinations[i];
      if(Cells[a].State != CellState::None && Cells[b].State != CellState::None && Cells[c].State != CellState::None 
          && Cells[a].State == Cells[b].State && Cells[b].State == Cells[c].State)
      {
        g_GameOver = true;

        switch(i)
        {
          case 0:
          case 1:
          case 2:
            DrawLineEx({Cells[a].Position.x, Cells[a].Position.y + Cells[a].Height / 2.0f }, {Cells[c].Position.x + Cells[c].Width, Cells[c].Position.y + Cells[c].Height / 2.0f}, 4.0f, RED);
            break;
          case 3:
          case 4:
          case 5:
            DrawLineEx({Cells[a].Position.x + Cells[a].Width / 2.0f, Cells[a].Position.y}, {Cells[c].Position.x + Cells[c].Width / 2.0f, Cells[c].Position.y + Cells[c].Height}, 4.0f, RED);
            break;
          case 6:
            DrawLineEx(Cells[a].Position, {Cells[c].Position.x + Cells[c].Width, Cells[c].Position.y + Cells[c].Height}, 4.0f, RED);
            break;
          case 7:
            DrawLineEx({Cells[a].Position.x + Cells[a].Width, Cells[a].Position.y}, {Cells[c].Position.x, Cells[c].Position.y + Cells[c].Height}, 4.0f, RED);
            break;
        }
      }

    }

    // UI
  
    if(!g_GameOver)
    {
      DrawText(PlayerTurnToString(), SCREEN_WIDTH - 100, 40, 20, FG_COLOR);
    }
    else
    {
      DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.5f));
      DrawText("Game Over", SCREEN_WIDTH / 2.f - 150, 80, 60, FG_COLOR);
      
      Button bRestart{{SCREEN_WIDTH / 2.f - 120, 160.f, 240.f, 40.f}, "Restart", [&](){
        ClearBoard();
        g_GameOver = false;
      }};
      bRestart.Update();
      bRestart.Draw();

      Button bExit{{SCREEN_WIDTH / 2.f - 120, 210.f, 240.f, 40.f}, "Exit", [&](){
        closeWindow = true;
      }};
      bExit.Update();
      bExit.Draw();
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
