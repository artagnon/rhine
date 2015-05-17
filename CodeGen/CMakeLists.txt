include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_BINARY_DIR}/Parse")
find_package(LLVM REQUIRED native mcjit)
link_directories(${LLVM_LIBRARY_DIRS})
add_library(CodeGen
  "${CMAKE_CURRENT_SOURCE_DIR}/Lowering.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/TypeLowering.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/TypeInfer.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Externals.cpp"
  "${PROJECT_SOURCE_DIR}/include/rhine/Diagnostic.h"
  "${PROJECT_SOURCE_DIR}/include/rhine/Externals.h"
  "${PROJECT_SOURCE_DIR}/include/rhine/Context.h"
  "${PROJECT_SOURCE_DIR}/include/rhine/Type.h"
  "${PROJECT_SOURCE_DIR}/include/rhine/Value.h"
  "${PROJECT_SOURCE_DIR}/include/rhine/IR.h")
set_target_properties(CodeGen PROPERTIES
  LINKER_LANGUAGE CXX
  COMPILE_FLAGS "${LLVM_CXXFLAGS}")
target_link_libraries(CodeGen
  ${LLVM_LDFLAGS}
  ${LLVM_LIBRARIES})
