# SFML 3.1 hard-codes m_highDpi = NO on macOS, so the GL view never gets a
# retina backing store and everything renders blurry at 1x. Flip it on.
# Runs as a FetchContent PATCH_COMMAND (cwd = SFML source dir); safe to re-run.

set(file "${CMAKE_CURRENT_SOURCE_DIR}/src/SFML/Window/macOS/SFWindowController.mm")

if(NOT EXISTS "${file}")
  message(FATAL_ERROR "high-DPI patch: ${file} not found — SFML layout changed?")
endif()

file(READ "${file}" content)

if(content MATCHES "m_highDpi       = YES;")
  return()
endif()

string(REPLACE "m_highDpi       = NO;" "m_highDpi       = YES;" patched "${content}")

if(patched STREQUAL "${content}")
  message(FATAL_ERROR "high-DPI patch: pattern not found — check SFML version against this patch")
endif()

file(WRITE "${file}" "${patched}")
message(STATUS "Patched SFML for macOS high-DPI rendering")
