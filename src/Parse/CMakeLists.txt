find_package(FLEX 2.5.39 REQUIRED)
find_package(LLVM REQUIRED)
include_directories("/usr/local/opt/flex/include")
include_directories("${TOOLS_ROOT}/flex-2.6/include") # for linux, custom tarball
flex_target(Lexx
  "${CMAKE_CURRENT_SOURCE_DIR}/Lexer.ll"
  "${CMAKE_CURRENT_BINARY_DIR}/Lexer.cpp")
include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_library(Parse
  ${FLEX_Lexx_OUTPUTS}
  "${CMAKE_CURRENT_SOURCE_DIR}/ParseInstruction.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ParseStatement.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ParseFunction.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ParseBlock.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ParseType.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Parser.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ParseDriver.cpp")
set_target_properties(Parse PROPERTIES
  LINKER_LANGUAGE CXX
  COMPILE_FLAGS "${LLVM_CXXFLAGS} -Wno-deprecated-register -Wno-sign-compare -fexceptions")
target_link_libraries(Parse)
