include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_BINARY_DIR}/Parse")
find_package(LLVM REQUIRED)
add_library(Typing
  "${CMAKE_CURRENT_SOURCE_DIR}/TypeInfer.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/TypeCoercion.cpp"
  "${PROJECT_SOURCE_DIR}/include/rhine/Resolve.h"
  "${PROJECT_SOURCE_DIR}/include/rhine/TypeCoercion.h"
  "${PROJECT_SOURCE_DIR}/include/rhine/Externals.h")
set_target_properties(Typing PROPERTIES
  LINKER_LANGUAGE CXX
  COMPILE_FLAGS "${LLVM_CXXFLAGS}")
