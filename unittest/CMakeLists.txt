find_package(LLVM REQUIRED)
find_package(CLANG REQUIRED)
include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_BINARY_DIR}/Parse")
include_directories("${PROJECT_SOURCE_DIR}/gtest/include")
include_directories("${LLVM_INCLUDE_DIRS}")
include_directories("${CLANG_INCLUDE_DIRS}")

add_executable(UnitTest
  "${CMAKE_CURRENT_SOURCE_DIR}/IRTest.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/CodeGenTest.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ExecutionTest.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/DiagClangTest.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/DiagnosticTest.cpp"
  "${PROJECT_SOURCE_DIR}/include/rhine/StdCapture.h")
set_target_properties(UnitTest PROPERTIES
  LINKER_LANGUAGE CXX
  COMPILE_FLAGS "${LLVM_CXXFLAGS} ${CLANG_CXXFLAGS}")
target_link_libraries(UnitTest
  gtest_main
  CodeGen
  Toplevel
  ${CLANG_LIBRARIES})
