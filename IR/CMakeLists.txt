include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_BINARY_DIR}/Parse")
find_package(LLVM REQUIRED)
link_directories(${LLVM_LIBRARY_DIRS})
add_library(IR
  "${CMAKE_CURRENT_SOURCE_DIR}/Value.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/UnresolvedValue.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/GlobalValue.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Type.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Constant.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/BasicBlock.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Instruction.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Module.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/User.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Use.cpp")
set_target_properties(IR PROPERTIES
  LINKER_LANGUAGE CXX
  COMPILE_FLAGS "${LLVM_CXXFLAGS}")
target_link_libraries(IR
  ${LLVM_LDFLAGS}
  ${LLVM_LIBRARIES})
add_dependencies(IR
  Parse)
