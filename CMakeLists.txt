# =============================================================================
# Project      : main (OpenGL 3D Learning)
# Version      : 1.6
# Author       : Muhammad Hassnain Khichi (aka drghost)
# Description  : Statically links ENet (libenet.a) and Raylib (libraylib.a)
#               Configures build types, compiler flags, and packaging targets
# =============================================================================

cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(main VERSION 1.6 LANGUAGES CXX)

#───────────────────────────────────────────────────────────────────────────────
# 1. Global Configuration
#───────────────────────────────────────────────────────────────────────────────

# Enforce C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default to Release if no build type was specified
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build (Debug or Release)" FORCE)
endif()

# Place executables in project root (parent of build/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Keep archives and libraries default or set elsewhere if needed
unset(CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
unset(CMAKE_LIBRARY_OUTPUT_DIRECTORY)

#───────────────────────────────────────────────────────────────────────────────
# 2. Sources
#───────────────────────────────────────────────────────────────────────────────

# Gather all .cpp files under src/
file(GLOB_RECURSE APP_SOURCES
     "${CMAKE_SOURCE_DIR}/src/*.cpp")

#───────────────────────────────────────────────────────────────────────────────
# 3. Executable Target
#───────────────────────────────────────────────────────────────────────────────

add_executable(${PROJECT_NAME}
    ${APP_SOURCES}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
      ${CMAKE_SOURCE_DIR}/include
)

#───────────────────────────────────────────────────────────────────────────────
# 4. Compiler & Linker Flags
#───────────────────────────────────────────────────────────────────────────────

target_compile_options(${PROJECT_NAME} PRIVATE
    -m64
    -Wall
    -Wextra
    -Wpedantic
    $<$<CONFIG:Debug>:-g -O0 -DDEBUG>
    $<$<CONFIG:Release>:-O2 -DNDEBUG>
)

#───────────────────────────────────────────────────────────────────────────────
# 5. Imported Static Libraries
#───────────────────────────────────────────────────────────────────────────────

add_library(raylib STATIC IMPORTED GLOBAL)
set_target_properties(raylib PROPERTIES
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/lib/libraylib.a"
)

add_library(enet STATIC IMPORTED GLOBAL)
set_target_properties(enet PROPERTIES
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/lib/libenet.a"
)

#───────────────────────────────────────────────────────────────────────────────
# 6. Linkage
#───────────────────────────────────────────────────────────────────────────────

target_link_libraries(${PROJECT_NAME} PRIVATE
    raylib
    enet
    gdi32
    winmm
    opengl32
    ws2_32
)

#───────────────────────────────────────────────────────────────────────────────
# 7. Custom Targets: Strip & Package
#───────────────────────────────────────────────────────────────────────────────

# Strip symbols from the final exe
add_custom_target(strip_symbols
    COMMAND ${CMAKE_STRIP} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.exe"
    COMMENT "∘ Stripping debugging symbols"
)

# Package into dist/
add_custom_target(package
    DEPENDS ${PROJECT_NAME} strip_symbols
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/dist"
    COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.exe"
            "${CMAKE_SOURCE_DIR}/dist/"
    COMMENT "∘ Packaging stripped executable into dist/"
)

# All-in-one release build + package
add_custom_target(release_package
    COMMENT "∘ Full Release → Build, Strip, Package"
    COMMAND ${CMAKE_COMMAND} --build . --config Release
    COMMAND ${CMAKE_COMMAND} --build . --target package
)

#───────────────────────────────────────────────────────────────────────────────
# 8. Usage Instructions
#───────────────────────────────────────────────────────────────────────────────

# To build:
#   mkdir build && cd build
#   cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
#   cmake --build . --config Release
#
# To strip & package:
#   cmake --build . --target release_package
#
# Final executable lives in ../main.exe (and dist/main.exe once packaged)
