cmake_minimum_required(VERSION 3.10)
project(synth)
set(SRC_DIR "src")

set(SRC_FILES 
  "${SRC_DIR}/main.c"
  "${SRC_DIR}/oscillator.c"
)

find_library(SOUNDIO
  NAMES soundio)

find_library(MATH
  NAMES m)

find_library(RAYLIB
  NAMES raylib)

add_executable(synth ${SRC_FILES})
target_link_libraries(synth ${SOUNDIO} ${MATH} ${RAYLIB})
