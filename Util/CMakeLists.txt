find_package(LLVM REQUIRED)
include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_BINARY_DIR}/Parse")
include_directories("${PROJECT_SOURCE_DIR}/gtest/include")
include_directories("${LLVM_INCLUDE_DIRS}")

add_library(Util
  "${PROJECT_SOURCE_DIR}/include/rhine/TestUtil.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/TestUtil.cpp")
set_target_properties(Util PROPERTIES
  LINKER_LANGUAGE CXX
  COMPILE_FLAGS "${LLVM_CXXFLAGS}")
target_link_libraries(Util
  gtest_main
  CodeGen
  Toplevel)
