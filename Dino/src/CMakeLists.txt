cmake_minimum_required(VERSION 3.24)
project(Dino_Clion C)

set(CMAKE_C_STANDARD 11)

link_libraries(SDL2 SDL2_image SDL2_ttf winmm)

add_executable(Dino_Clion GameFrame.c Background.c Background.h Dino.c Dino.h Fireball.c Fireball.h Projectile.c Projectile.h Resource.c Resource.h)
