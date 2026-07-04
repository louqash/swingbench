# --- Basics -----------------------------------------------------------------
cmake_minimum_required(VERSION 3.28)   # a safe modern baseline
project(swingbench CXX)                # project name + language (CXX = C++)

# Always emit build/compile_commands.json so clangd (nvim) knows the include
# paths (SFML, etc.). Baked in here so it survives clearing the build dir.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_library(pendulum
  src/pendulum.cpp
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

target_include_directories(pendulum PUBLIC include)

target_compile_options(pendulum PRIVATE -Wall -Wextra)
target_compile_features(pendulum PRIVATE cxx_std_20)

# --- The runnable app -------------------------------------------------------
add_executable(swingbench src/main.cpp src/renderer.cpp)
target_link_libraries(swingbench PRIVATE SFML::Graphics pendulum)
