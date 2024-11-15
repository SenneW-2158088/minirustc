cmake_minimum_required(VERSION 3.10)
project(MiniRustCompiler CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(FLEX 2.5.13 REQUIRED)
find_package(BISON 3.2 REQUIRED)

if(APPLE)
  set(FLEX_USE_STATIC_LIB ON)
endif()


file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/src/lexer)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/src/parser)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/lexer)# Generate the lexer
flex_target(Lexer
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lexer/lexer.c
    ${CMAKE_CURRENT_BINARY_DIR}/src/lexer/lexer.cpp
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/src/lexer/lexer.h
)

# Generate the parser
bison_target(Parser
    ${CMAKE_CURRENT_SOURCE_DIR}/src/parser/parser.c
    ${CMAKE_CURRENT_BINARY_DIR}/src/parser/parser.cpp
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/src/parser/parser.h
)

message(STATUS "Flex output: ${FLEX_Lexer_OUTPUTS}")
message(STATUS "Bison output: ${BISON_Parser_OUTPUTS}")

# Add Flex and Bison outputs to the list of generated sources
add_flex_bison_dependency(Lexer Parser)

# Define the generated sources
set(GENERATED_SOURCES
    ${FLEX_Lexer_OUTPUTS}
    ${BISON_Parser_OUTPUTS}
)

set(DRIVER
    ${CMAKE_CURRENT_SOURCE_DIR}/src/driver/Driver.cpp
)
# Main executable
add_executable(mrc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${DRIVER}
    ${GENERATED_SOURCES}
)

target_include_directories(mrc PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}/src
)
