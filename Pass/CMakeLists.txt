include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_BINARY_DIR}/Parse")
find_package(LLVM REQUIRED)
add_library(Pass
  "${CMAKE_CURRENT_SOURCE_DIR}/FunctionPass.cpp")
set_target_properties(Pass PROPERTIES
  LINKER_LANGUAGE CXX
  COMPILE_FLAGS "${LLVM_CXXFLAGS}")
target_link_libraries(Pass
  ${LLVM_LDFLAGS}
  ${LLVM_LIBRARIES})
