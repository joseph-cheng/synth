cmake_minimum_required(VERSION 3.10)
project(synth)
set(SRC_DIR "src")

set(SRC_FILES 
  "${SRC_DIR}/main.c"
  "${SRC_DIR}/oscillator.c"
  "${SRC_DIR}/ui_manager.c"
  "${SRC_DIR}/ui_widget.c"
  "${SRC_DIR}/dynamic_array.c"
  "${SRC_DIR}/synth.c"
  "${SRC_DIR}/filter.c"
)

find_library(SOUNDIO
  NAMES soundio)

find_library(MATH
  NAMES m)

find_library(RAYLIB
  NAMES raylib)

add_executable(synth ${SRC_FILES})
target_link_libraries(synth ${SOUNDIO} ${MATH} ${RAYLIB})
