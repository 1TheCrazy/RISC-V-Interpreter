cmake_minimum_required(VERSION 3.20)
project(MyProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Auto discover source files inside src/ ---
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

# --- Add main.cpp manually ---
add_executable(riscv-interpreter
    main.cpp
    ${SOURCES}
)

# --- Include headers ---
target_include_directories(riscv-interpreter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)