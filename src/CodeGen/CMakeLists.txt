find_package(LLVM REQUIRED native mcjit)

include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_BINARY_DIR}/src/Parse")
link_directories(${LLVM_LIBRARY_DIRS})
add_library(CodeGen
  "${CMAKE_CURRENT_SOURCE_DIR}/TypeLowering.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/UnresolvedValueLowering.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/GlobalValueLowering.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ConstantLowering.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/TensorLowering.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/FunctionLowering.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/InstLowering.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ModuleLowering.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/BasicBlockLowering.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Externals.cpp")
set_target_properties(CodeGen PROPERTIES
  LINKER_LANGUAGE CXX
  COMPILE_FLAGS "${LLVM_CXXFLAGS}")
target_link_libraries(CodeGen
  IR
  ${LLVM_LDFLAGS}
  ${LLVM_LIBRARIES})
