find_package(LLVM REQUIRED)

include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_BINARY_DIR}/Parse")

add_library(Diagnostic
  "${CMAKE_CURRENT_SOURCE_DIR}/DiagnosticPrinter.cpp"
  "${PROJECT_SOURCE_DIR}/include/rhine/Diagnostic.h")
set_target_properties(Diagnostic PROPERTIES
  LINKER_LANGUAGE CXX
  COMPILE_FLAGS "${LLVM_CXXFLAGS}")
