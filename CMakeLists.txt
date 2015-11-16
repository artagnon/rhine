cmake_minimum_required(VERSION 2.6)
project(Rhine)

# set(CMAKE_CXX_COMPILER "/usr/local/bin/clang++")
# set(CMAKE_CXX_FLAGS "-fsanitize=address")
set(CMAKE_CXX_FLAGS "-std=c++14")
set(LLVM_ROOT_DIR "${PROJECT_SOURCE_DIR}/llvm")
set(CLANG_ROOT_DIR "${PROJECT_SOURCE_DIR}/clang")
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/Modules")

include_directories("${PROJECT_SOURCE_DIR}/include")
add_subdirectory(src/Parse)
add_subdirectory(src/Pass)
add_subdirectory(src/Transform)
add_subdirectory(src/IR)
add_subdirectory(src/CodeGen)
add_subdirectory(src/Runtime)
add_subdirectory(src/Toplevel)
add_subdirectory(src/Diagnostic)
add_subdirectory(src/UI)
add_subdirectory(gtest)
add_subdirectory(unittest)

add_custom_target(check
  COMMAND "${PROJECT_BINARY_DIR}/unittest/UnitTest"
  COMMENT "Running unittests ..."
  DEPENDS CLI
  DEPENDS UnitTest)
