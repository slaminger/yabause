project(m68kmake)

cmake_minimum_required(VERSION 2.6)

SET(CMAKE_C_COMPILER gcc)
SET(CMAKE_C_FLAGS "" )

add_executable(m68kmake m68kmake.c)

if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif (MSVC)

add_custom_command(OUTPUT m68kops.h
                   COMMAND m68kmake ${CMAKE_CURRENT_BINARY_DIR}/ ${CMAKE_CURRENT_SOURCE_DIR}/m68k_in.c
                   DEPENDS m68kmake.c
                   COMMENT "Generating musashi 68k core"
                   VERBATIM)

add_custom_target(musashi ALL DEPENDS m68kops.h)
