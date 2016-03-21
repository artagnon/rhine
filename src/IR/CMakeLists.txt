find_package(LLVM REQUIRED)

include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_BINARY_DIR}/src/Parse")
link_directories(${LLVM_LIBRARY_DIRS})
add_library(IR
  "${CMAKE_CURRENT_SOURCE_DIR}/Value.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/UnresolvedValue.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/GlobalValue.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Type.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Constant.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Tensor.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Function.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/BasicBlock.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Instruction.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Module.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/User.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Use.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Context.cpp")
set_target_properties(IR PROPERTIES
  LINKER_LANGUAGE CXX
  COMPILE_FLAGS "${LLVM_CXXFLAGS}")
target_link_libraries(IR
  CodeGen)
add_dependencies(IR
  Parse
  CodeGen)
