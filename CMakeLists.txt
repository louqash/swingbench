# --- Basics -----------------------------------------------------------------
cmake_minimum_required(VERSION 3.28)   # a safe modern baseline
project(pendulum_sim CXX)              # project name + language (CXX = C++)

# Always emit build/compile_commands.json so clangd (nvim) knows the include
# paths (SFML, etc.). Baked in here so it survives clearing the build dir.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# --- The runnable app -------------------------------------------------------
add_executable(sim src/main.cpp)
target_link_libraries(sim PRIVATE SFML::Graphics)
