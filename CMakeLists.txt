cmake_minimum_required(VERSION 3.16)
project(DogeshBangla VERSION 1.0 LANGUAGES CXX)

# ── C++17 required (for std::filesystem) ─────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Output binary name ────────────────────────────────────────────
set(COMPILER_TARGET dogeshbangla)

# ── Source files ──────────────────────────────────────────────────
set(SOURCES
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/semantic.cpp
    src/codegen.cpp
)

# ── Executable ────────────────────────────────────────────────────
add_executable(${COMPILER_TARGET} ${SOURCES})

# ── Include directories ───────────────────────────────────────────
target_include_directories(${COMPILER_TARGET} PRIVATE src)

# ── Compiler warnings ─────────────────────────────────────────────
if(MSVC)
    target_compile_options(${COMPILER_TARGET} PRIVATE /W4)
else()
    target_compile_options(${COMPILER_TARGET} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ── Link filesystem library (GCC < 9 needs explicit link) ─────────
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
    target_link_libraries(${COMPILER_TARGET} stdc++fs)
endif()

# ── Install ───────────────────────────────────────────────────────
install(TARGETS ${COMPILER_TARGET} DESTINATION bin)

# ── Print config summary ──────────────────────────────────────────
message(STATUS "==============================================")
message(STATUS " DogeshBangla Compiler v1.0")
message(STATUS " Build type : ${CMAKE_BUILD_TYPE}")
message(STATUS " C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS " Binary name : ${COMPILER_TARGET}")
message(STATUS "==============================================")
