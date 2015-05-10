find_package(LLVM REQUIRED)
find_package(CLANG REQUIRED)
include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_BINARY_DIR}/Parse")
include_directories("${LLVM_INCLUDE_DIRS}")
include_directories("${CLANG_INCLUDE_DIRS}")

add_library(Toplevel
  "${CMAKE_CURRENT_SOURCE_DIR}/ParseFacade.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/JitFacade.cpp"
  "${PROJECT_SOURCE_DIR}/include/rhine/IR.h"
  "${PROJECT_SOURCE_DIR}/include/rhine/Toplevel.h")
set_target_properties(Toplevel PROPERTIES
  LINKER_LANGUAGE CXX
  COMPILE_FLAGS "${LLVM_CXXFLAGS} ${CLANG_CXXFLAGS}")
target_link_libraries(Toplevel
  CodeGen
  Parse)
