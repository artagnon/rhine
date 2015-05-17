include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_BINARY_DIR}/Parse")
find_package(LLVM REQUIRED native mcjit)
link_directories(${LLVM_LIBRARY_DIRS})
add_library(IR
  "${CMAKE_CURRENT_SOURCE_DIR}/Value.cpp"
  "${PROJECT_SOURCE_DIR}/include/rhine/Context.h"
  "${PROJECT_SOURCE_DIR}/include/rhine/IR/Type.h"
  "${PROJECT_SOURCE_DIR}/include/rhine/IR/Value.h"
  "${PROJECT_SOURCE_DIR}/include/rhine/IR/Constant.h"
  "${PROJECT_SOURCE_DIR}/include/rhine/IR/Instruction.h"
  "${PROJECT_SOURCE_DIR}/include/rhine/IR/Module.h"
  "${PROJECT_SOURCE_DIR}/include/rhine/IR.h")
set_target_properties(IR PROPERTIES
  LINKER_LANGUAGE CXX
  COMPILE_FLAGS "${LLVM_CXXFLAGS}")
target_link_libraries(IR
  ${LLVM_LDFLAGS}
  ${LLVM_LIBRARIES})
