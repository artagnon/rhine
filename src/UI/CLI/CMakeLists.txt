find_package(LLVM REQUIRED)

add_executable(CLI
  "${CMAKE_CURRENT_SOURCE_DIR}/Rhine.cpp")

set_target_properties(CLI PROPERTIES
  OUTPUT_NAME "rhine"
  RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
  LINKER_LANGUAGE CXX
  COMPILE_FLAGS "${LLVM_CXXFLAGS}")

target_link_libraries(CLI
  Toplevel)
