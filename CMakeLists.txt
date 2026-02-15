cmake_minimum_required(VERSION 4.2)

project(TicTacToeRaylib)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(vendor/raylib)

add_executable(TicTacToeRaylib)

target_sources(TicTacToeRaylib
  PRIVATE
    src/main.cpp
)

target_link_libraries(TicTacToeRaylib 
  PRIVATE
    raylib
)
