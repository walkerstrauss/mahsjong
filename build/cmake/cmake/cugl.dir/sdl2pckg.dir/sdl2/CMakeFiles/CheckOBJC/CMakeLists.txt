cmake_minimum_required(VERSION 3.31.5)
set(CMAKE_MODULE_PATH "/Users/patrickchoo/cs4152/cugl/sdlapp/buildfiles/cmake/sdl2/modules")
project(CheckOBJC OBJC)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/result.cmake"
  "set(CMAKE_OBJC_COMPILER \"${CMAKE_OBJC_COMPILER}\")\n"
  
  )