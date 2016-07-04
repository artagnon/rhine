cmake_minimum_required(VERSION 2.6)
project(Rhine)

set(CMAKE_CXX_FLAGS "-Werror -std=c++14")
# "-fprofile-instr-generate -fcoverage-mapping")
set(LLVM_ROOT_DIR "${PROJECT_SOURCE_DIR}/llvm")
set(CLANG_ROOT_DIR "${PROJECT_SOURCE_DIR}/clang")
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/Modules")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(UNITTEST_EXE "${PROJECT_BINARY_DIR}/unittest/UnitTest")
set(VALGRIND_EXE "/usr/local/bin/valgrind")

include_directories("${PROJECT_SOURCE_DIR}/include")
add_subdirectory(src/Parse)
add_subdirectory(src/Pass)
add_subdirectory(src/Transform)
add_subdirectory(src/IR)
add_subdirectory(src/CodeGen)
add_subdirectory(src/Toplevel)
add_subdirectory(src/UI)
add_subdirectory(gtest)
add_subdirectory(unittest)

add_custom_target(check "${UNITTEST_EXE}"
#  COMMAND "xcrun" "llvm-profdata" "merge"
#  "-o" "${PROJECT_BINARY_DIR}/default.profdata"
#  "${PROJECT_BINARY_DIR}/default.profraw"
  COMMENT "Running unittests ..."
  DEPENDS CLI
  DEPENDS UnitTest
  USES_TERMINAL)

add_custom_target(valgrind
  COMMAND "${VALGRIND_EXE}" "--" "${UNITTEST_EXE}" "--gtest_filter=Valgrind.*"
  COMMENT "Running valgrind tests ..."
  DEPENDS CLI
  DEPENDS UnitTest)

add_custom_target(valgrind-all
  COMMAND "${VALGRIND_EXE}" "${UNITTEST_EXE}"
  COMMENT "Running valgrind comprehensive tests ..."
  DEPENDS CLI
  DEPENDS UnitTest)
