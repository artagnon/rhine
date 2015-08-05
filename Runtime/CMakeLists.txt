include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_BINARY_DIR}/Parse")
find_package(LLVM REQUIRED native mcjit)
link_directories(${LLVM_LIBRARY_DIRS})
add_library(Runtime
  "${CMAKE_CURRENT_SOURCE_DIR}/GC.cpp")
set_target_properties(Runtime PROPERTIES
  LINKER_LANGUAGE CXX
  COMPILE_FLAGS "${LLVM_CXXFLAGS}")
target_link_libraries(Runtime
  IR
  ${LLVM_LDFLAGS}
  ${LLVM_LIBRARIES})
